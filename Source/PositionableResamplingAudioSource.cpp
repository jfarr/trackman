#include "PositionableResamplingAudioSource.h"

PositionableResamplingAudioSource::PositionableResamplingAudioSource(juce::PositionableAudioSource *source,
    const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels)
    : source(source), resamplerSource(source, false, maxNumChannels), sourceSampleRate(sourceSampleRateToCorrectFor),
      deleteWhenRemoved(deleteWhenRemoved) {}

PositionableResamplingAudioSource::~PositionableResamplingAudioSource() {
    if (deleteWhenRemoved && source != nullptr) {
        std::unique_ptr<AudioSource> toDelete;
        toDelete.reset(source);
    }
}

//==============================================================================
void PositionableResamplingAudioSource::prepareToPlay(int samplesPerBlockExpected, double newSampleRate) {
    // const juce::ScopedLock sl(callbackLock);

    sampleRate = newSampleRate;
    blockSize = samplesPerBlockExpected;

    resamplerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    if (sourceSampleRate > 0)
        resamplerSource.setResamplingRatio(sourceSampleRate / sampleRate);
}

void PositionableResamplingAudioSource::releaseResources() {
    // const juce::ScopedLock sl(callbackLock);
    resamplerSource.releaseResources();
}

void PositionableResamplingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &info) {
    // const juce::ScopedLock sl(callbackLock);
    resamplerSource.getNextAudioBlock(info);
}

//==============================================================================
void PositionableResamplingAudioSource::setNextReadPosition(juce::int64 newPosition) {
    source->setNextReadPosition((juce::int64)(newPosition * getSampleRatio()));
}

juce::int64 PositionableResamplingAudioSource::getNextReadPosition() const {
    return (double)source->getNextReadPosition() * getSampleRatio();
}

juce::int64 PositionableResamplingAudioSource::getTotalLength() const {
    // const juce::ScopedLock sl(callbackLock);

    return (juce::int64)((double)source->getTotalLength() * getSampleRatio());
}

bool PositionableResamplingAudioSource::isLooping() const { return source->isLooping(); }

void PositionableResamplingAudioSource::setLooping(bool shouldLoop) { source->setLooping(shouldLoop); }
