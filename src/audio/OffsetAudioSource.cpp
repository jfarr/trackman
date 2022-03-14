#include "OffsetAudioSource.h"

OffsetAudioSource::OffsetAudioSource(PositionableAudioSource &source, double offsetSeconds, double sampleRate)
    : source(source), sampleRate(sampleRate), offsetSamples(offsetSeconds * sampleRate),
      minLength(source.getTotalLength()) {}

OffsetAudioSource::~OffsetAudioSource() {}

void OffsetAudioSource::setOffsetSeconds(double offsetSeconds) {
    auto currentPos = getNextReadPosition();
    if (true) {
        const juce::ScopedLock lock(mutex);
        offsetSamples = offsetSeconds * sampleRate;
    }
    setNextReadPosition(currentPos);
}

void OffsetAudioSource::setMinLength(juce::int64 newLength) {
    const juce::ScopedLock lock(mutex);
    minLength = newLength;
}

void OffsetAudioSource::prepareToPlay(int blockSize, double sampleRate) { source.prepareToPlay(blockSize, sampleRate); }

void OffsetAudioSource::releaseResources() {}

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
    auto len = source.getTotalLength() + offsetSamples;
    auto clamped = std::max(len, minLength);
    DBG("offset source (min " << minLength << ") len: " << clamped);
    return clamped;
}

bool OffsetAudioSource::isLooping() const { return false; }

void OffsetAudioSource::setLooping(bool shouldLoop) {}
