#include "Mixer.h"

Mixer::Mixer() {
    transportSource.setSource(&mixerSource);
}

Mixer::~Mixer() {
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
}

void Mixer::addSource(std::shared_ptr<juce::PositionableAudioSource> source, const bool deleteWhenRemoved,
    double sourceSampleRateToCorrectFor, int maxNumChannels) {
    sources.push_back(source);
    mixerSource.addInputSource(source.get(), false, sourceSampleRateToCorrectFor, maxNumChannels);
}

void Mixer::removeAllSources() {
    for (std::shared_ptr<juce::PositionableAudioSource> &source : sources) {
        mixerSource.removeInputSource(source.get());
    }
    sources.clear();
}

void Mixer::setMasterLevelGain(float newLevel) {
    level = newLevel;
    transportSource.setGain(newLevel);
}

void Mixer::toggleMasterMute() {
    muted = !muted;
    transportSource.setGain((muted ? 0 : level));
}

void Mixer::setMasterMute(bool newMuted) {
    muted = newMuted;
    transportSource.setGain((muted ? 0 : level));
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void Mixer::releaseResources() { transportSource.releaseResources(); }
