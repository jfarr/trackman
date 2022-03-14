#include "OffsetAudioSource.h"

OffsetAudioSource::OffsetAudioSource(PositionableAudioSource &source, double offsetSeconds, double sampleRate)
    : source(source), sampleRate(sampleRate), offsetSamples(offsetSeconds * sampleRate) {}

OffsetAudioSource::~OffsetAudioSource() {}

void OffsetAudioSource::setOffsetSeconds(double offsetSeconds) {
    const juce::ScopedLock lock(mutex);
    auto currentPos = getNextReadPosition();
    offsetSamples = offsetSeconds * sampleRate;
    setNextReadPosition(currentPos);
}

void OffsetAudioSource::prepareToPlay(int blockSize, double sampleRate) { source.prepareToPlay(blockSize, sampleRate); }

void OffsetAudioSource::releaseResources() { source.releaseResources(); }

void OffsetAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    source.getNextAudioBlock(bufferToFill);
}

void OffsetAudioSource::setNextReadPosition(juce::int64 newPosition) {
    const juce::ScopedLock lock(mutex);
    source.setNextReadPosition(newPosition - offsetSamples);
}

juce::int64 OffsetAudioSource::getNextReadPosition() const {
    const juce::ScopedLock lock(mutex);
    return source.getNextReadPosition() + offsetSamples;
}

juce::int64 OffsetAudioSource::getTotalLength() const {
    const juce::ScopedLock lock(mutex);
    return source.getTotalLength() + offsetSamples;
}

bool OffsetAudioSource::isLooping() const { return false; }

void OffsetAudioSource::setLooping(bool shouldLoop) {}
