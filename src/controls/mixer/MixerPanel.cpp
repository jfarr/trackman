#include "MixerPanel.h"
#include "common/listutil.h"

MixerPanel::MixerPanel() : transportControl(transportSource) {
    setAudioChannels(0, 2);
    transportSource.setSource(&mixerSource);
    transportControl.addListener(this);
    masterTrackControl.addListener(this);
    createControls();
    setSize(800, 250);
}

MixerPanel::~MixerPanel() {
    shutdownAudio();
    masterTrackControl.removeListener(this);
    transportControl.removeListener(this);
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
    for (auto &source : sources)
        delete source;
    sources.clear();
}

void MixerPanel::createControls() {
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
}

void MixerPanel::setMasterLevel(float newLevel) {
    level = newLevel;
    masterTrackControl.setLevel(level);
}

void MixerPanel::onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
    const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels) {
    if (prevSource != nullptr) {
        mixerSource.removeInputSource(prevSource);
    }
    mixerSource.addInputSource(newSource, deleteWhenRemoved, sourceSampleRateToCorrectFor);
}

void MixerPanel::loopingChanged(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

void MixerPanel::masterLevelChanged(float newLevel) {
    level = newLevel;
    transportSource.setGain(newLevel);
}

void MixerPanel::masterMuteToggled() {
    muted = !muted;
    auto newLevel = (muted ? 0.0 : level);
    transportSource.setGain(newLevel);
}

//==============================================================================
void MixerPanel::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixerPanel::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void MixerPanel::releaseResources() { transportSource.releaseResources(); }

//==============================================================================
void MixerPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
    auto buttonHeight = 35;
    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(buttonHeight).reduced(transportMargin));
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getWidth()));
    notifyResized(area);
}

void MixerPanel::addListener(MixerPanelListener *listener) {
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void MixerPanel::removeListener(MixerPanelListener *listener) { listeners.remove(listener); }

void MixerPanel::notifyResized(juce::Rectangle<int> area) {
    for (MixerPanelListener *listener : listeners) {
        listener->mixerResized(area);
    }
}
