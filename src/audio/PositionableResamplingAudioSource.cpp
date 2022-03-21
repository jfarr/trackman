#include "PositionableResamplingAudioSource.h"

namespace trackman {

PositionableResamplingAudioSource::PositionableResamplingAudioSource(PositionableAudioSource *source,
    const bool deleteWhenRemoved, double sampleRate, double sourceSampleRateToCorrectFor, int maxNumChannels)
    : source(source), resamplerSource(source, false, maxNumChannels), sampleRate(sampleRate),
      sourceSampleRate(sourceSampleRateToCorrectFor), deleteWhenRemoved(deleteWhenRemoved) {
    if (sourceSampleRate > 0) {
        resamplerSource.setResamplingRatio(sourceSampleRate / sampleRate);
    }
}

PositionableResamplingAudioSource::~PositionableResamplingAudioSource() {
    if (deleteWhenRemoved && source != nullptr) {
        unique_ptr<AudioSource> toDelete;
        toDelete.reset(source);
    }
}

void PositionableResamplingAudioSource::setSourceSampleRateToCorrectFor(double newSampleRate) {
    const ScopedLock lock(mutex);
    sourceSampleRate = newSampleRate;
    if (sourceSampleRate > 0) {
        resamplerSource.setResamplingRatio(sourceSampleRate / sampleRate);
    }
}

//==============================================================================
void PositionableResamplingAudioSource::prepareToPlay(int samplesPerBlockExpected, double newSampleRate) {
    const ScopedLock lock(mutex);
    sampleRate = newSampleRate;
    blockSize = samplesPerBlockExpected;

    resamplerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    if (sourceSampleRate > 0) {
        resamplerSource.setResamplingRatio(sourceSampleRate / sampleRate);
    }
}

void PositionableResamplingAudioSource::releaseResources() {
    if (deleteWhenRemoved) {
        resamplerSource.releaseResources();
    }
}

void PositionableResamplingAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &info) {
    resamplerSource.getNextAudioBlock(info);
}

//==============================================================================
void PositionableResamplingAudioSource::setNextReadPosition(int64 newPosition) {
    source->setNextReadPosition(newPosition * getSampleRatio());
}

int64 PositionableResamplingAudioSource::getNextReadPosition() const {
    return source->getNextReadPosition() * getSampleRatio();
}

int64 PositionableResamplingAudioSource::getTotalLength() const { return source->getTotalLength() * getSampleRatio(); }

bool PositionableResamplingAudioSource::isLooping() const { return source->isLooping(); }

void PositionableResamplingAudioSource::setLooping(bool shouldLoop) { source->setLooping(shouldLoop); }

} // namespace trackman
