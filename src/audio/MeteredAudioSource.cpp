#include "MeteredAudioSource.h"

MeteredAudioSource::MeteredAudioSource(double sampleRate) : sampleRate(sampleRate) {
    meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
}

MeteredAudioSource::~MeteredAudioSource() {

}

void MeteredAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
}

void MeteredAudioSource::releaseResources() { }

void MeteredAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    meterSource.measureBlock(*bufferToFill.buffer);
}
