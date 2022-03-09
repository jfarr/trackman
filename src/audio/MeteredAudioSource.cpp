#include "MeteredAudioSource.h"

MeteredAudioSource::MeteredAudioSource(juce::PositionableAudioSource &source, double sampleRate)
    : PositionableAudioSourceAdapter(&source, false), source(source), sampleRate(sampleRate) {
    meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
}

MeteredAudioSource::~MeteredAudioSource() {}

void MeteredAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    sampleRate = newSampleRate;
    meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
    source.prepareToPlay(blockSize, newSampleRate);
}

void MeteredAudioSource::releaseResources() { source.releaseResources(); }

void MeteredAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    meterSource.measureBlock(*bufferToFill.buffer);
}
