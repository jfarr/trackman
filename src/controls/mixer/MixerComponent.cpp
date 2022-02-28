#include "MixerComponent.h"

MixerComponent::MixerComponent(juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), transportControl(transportSource) {
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

void MixerComponent::addTrack(Track &track) {
    tracks.push_back(&track);
    removeAllChildren();
    createControls();
    for (std::list<Track *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        Track &t = **i;
        addAndMakeVisible(t.getTrackControl());
        t.getTrackControl().setListener(&t);
    }
    resized();
}

void MixerComponent::onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
    const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels) {
    if (prevSource != nullptr) {
        mixerSource.removeInputSource(prevSource);
    }
    mixerSource.addInputSource(newSource, deleteWhenRemoved, sourceSampleRateToCorrectFor);
}

void MixerComponent::updateLoopState(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

void MixerComponent::levelChanged(float level) { transportSource.setGain(level); }

void MixerComponent::muteChanged(bool muted) {}

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
    for (std::list<Track *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        Track &track = **i;
        track.getTrackControl().setBounds(area.removeFromLeft(track.getTrackControl().getWidth()));
    }
}