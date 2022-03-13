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

void MeteredAudioSource::releaseResources() {}

void MeteredAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    source.getNextAudioBlock(bufferToFill);
    meterSource.measureBlock(*bufferToFill.buffer);
    DBG("play length: " << source.getTotalLength());
    DBG("play length / 48000: " << source.getTotalLength() / 48000.0);
    DBG("play length / 44100: " << source.getTotalLength() / 44100.0);
}
