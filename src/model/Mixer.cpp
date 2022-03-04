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

void Mixer::removeSource(std::shared_ptr<juce::PositionableAudioSource> source) {
    mixerSource.removeInputSource(source.get());
    sources.remove(source);
}

void Mixer::setMasterLevel(float newLevel) { level = newLevel; }

void Mixer::toggleMasterMute() {
    muted = !muted;
    auto newLevel = (muted ? 0.0 : level);
    transportSource.setGain(newLevel);
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void Mixer::releaseResources() { transportSource.releaseResources(); }
