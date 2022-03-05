#include "OffsetAudioSource.h"

OffsetAudioSource::OffsetAudioSource(PositionableAudioSource &source, juce::int64 offsetSeconds) : source(source), offsetSeconds(offsetSeconds) {}

OffsetAudioSource::~OffsetAudioSource() {}

void OffsetAudioSource::prepareToPlay(int blockSize, double sampleRate) { source.prepareToPlay(blockSize, sampleRate); }

void OffsetAudioSource::releaseResources() { source.releaseResources(); }

void OffsetAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    source.getNextAudioBlock(bufferToFill);
}

void OffsetAudioSource::setNextReadPosition(juce::int64 newPosition) {
    source.setNextReadPosition(newPosition - offsetSeconds);
}

juce::int64 OffsetAudioSource::getNextReadPosition() const { return source.getNextReadPosition() + offsetSeconds; }

juce::int64 OffsetAudioSource::getTotalLength() const { return source.getTotalLength() + offsetSeconds; }

bool OffsetAudioSource::isLooping() const { return false; }

void OffsetAudioSource::setLooping(bool shouldLoop) {}
