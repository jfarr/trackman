#include "MeteredAudioSource.h"

namespace trackman {

MeteredAudioSource::MeteredAudioSource(PositionableAudioSource *source, double sampleRate)
    : PositionableAudioSourceAdapter(source, false), source(source), sampleRate(sampleRate) {
    if (sampleRate > 0) {
        meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
    }
}

MeteredAudioSource::~MeteredAudioSource() {}

void MeteredAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    sampleRate = newSampleRate;
    meterSource.resize(2, refreshRateHz * 0.001f * sampleRate / blockSize);
    if (source != nullptr) {
        source->prepareToPlay(blockSize, newSampleRate);
    }
}

void MeteredAudioSource::releaseResources() {}

void MeteredAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    if (source != nullptr) {
        source->getNextAudioBlock(bufferToFill);
    }
    meterSource.measureBlock(*bufferToFill.buffer);
}

} // namespace trackman
