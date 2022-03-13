#include "Mixer.h"

Mixer::Mixer(double sampleRate)
    : mixerSource(sampleRate), gainSource(&mixerSource, false), meteredSource(gainSource, sampleRate) {
    transportSource.setSource(&meteredSource);
    initialized = true;
}

Mixer::~Mixer() {
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
}

void Mixer::addSource(
    std::shared_ptr<juce::PositionableAudioSource> source, double sourceSampleRateToCorrectFor, int maxNumChannels) {
    DBG("Mixer::addSource - add source with length: " << source->getTotalLength());
    sources.push_back(source);
    auto pos = transportSource.getCurrentPosition();
    mixerSource.addInputSource(source.get(), false, sourceSampleRateToCorrectFor, maxNumChannels);
    transportSource.setPosition(pos);
    DBG("Mixer::addSource - set position: " << pos);
}

void Mixer::removeAllSources() {
    for (std::shared_ptr<juce::PositionableAudioSource> &source : sources) {
        mixerSource.removeInputSource(source.get());
    }
    sources.clear();
}

void Mixer::setMasterLevelGain(float newLevel) {
    level = newLevel;
    gainSource.setGain(newLevel);
}

void Mixer::setMasterMute(bool newMuted) {
    muted = newMuted;
    gainSource.setGain((muted ? 0 : level));
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    if (initialized) {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (initialized) {
        transportSource.getNextAudioBlock(bufferToFill);
    }
}

void Mixer::releaseResources() {
    if (initialized) {
        transportSource.releaseResources();
    }
}
