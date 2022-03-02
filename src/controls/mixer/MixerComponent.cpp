#include "MixerComponent.h"
#include "common/listutil.h"

MixerComponent::MixerComponent() : transportControl(transportSource) {
    setAudioChannels(0, 2);
    transportSource.setSource(&mixerSource);
    transportControl.addListener(this);
    masterTrackControl.addListener(this);
    createControls();
    setSize(800, 250);
}

MixerComponent::~MixerComponent() {
    shutdownAudio();
    masterTrackControl.removeListener(this);
    transportControl.removeListener(this);
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
    for (auto &source : sources)
        delete source;
    sources.clear();
}

void MixerComponent::createControls() {
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
}

void MixerComponent::setLevel(float newLevel) {
    level = newLevel;
    masterTrackControl.setLevel(level);
}

void MixerComponent::onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
    const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels) {
    if (prevSource != nullptr) {
        mixerSource.removeInputSource(prevSource);
    }
    mixerSource.addInputSource(newSource, deleteWhenRemoved, sourceSampleRateToCorrectFor);
}

void MixerComponent::updateLoopState(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

void MixerComponent::levelChanged(float newLevel) {
    level = newLevel;
    transportSource.setGain(newLevel);
}

void MixerComponent::muteToggled() {
    muted = !muted;
    auto newLevel = (muted ? 0.0 : level);
    transportSource.setGain(newLevel);
}

//==============================================================================
void MixerComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixerComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void MixerComponent::releaseResources() { transportSource.releaseResources(); }

//==============================================================================
void MixerComponent::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MixerComponent::resized() {
    auto area = getLocalBounds();
    auto buttonHeight = 35;
    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(buttonHeight).reduced(transportMargin));
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getWidth()));
    notifyResized(area);
}

void MixerComponent::addListener(MixerComponentListener *listener) {
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void MixerComponent::removeListener(MixerComponentListener *listener) { listeners.remove(listener); }

void MixerComponent::notifyResized(juce::Rectangle<int> area) {
    for (std::list<MixerComponentListener *>::iterator i = listeners.begin(); i != listeners.end(); ++i) {
        MixerComponentListener &listener = **i;
        listener.mixerResized(area);
    }
}
