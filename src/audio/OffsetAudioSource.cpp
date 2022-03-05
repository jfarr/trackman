#include "OffsetAudioSource.h"

OffsetAudioSource::OffsetAudioSource(PositionableAudioSource &source, double offsetSeconds, double sampleRate)
    : source(source), offsetSamples(offsetSeconds * sampleRate) {}

OffsetAudioSource::~OffsetAudioSource() {}

void OffsetAudioSource::prepareToPlay(int blockSize, double sampleRate) { source.prepareToPlay(blockSize, sampleRate); }

void OffsetAudioSource::releaseResources() { source.releaseResources(); }

void OffsetAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    source.getNextAudioBlock(bufferToFill);
}

void OffsetAudioSource::setNextReadPosition(juce::int64 newPosition) {
    source.setNextReadPosition(newPosition - offsetSamples);
}

juce::int64 OffsetAudioSource::getNextReadPosition() const { return source.getNextReadPosition() + offsetSamples; }

juce::int64 OffsetAudioSource::getTotalLength() const { return source.getTotalLength() + offsetSamples; }

bool OffsetAudioSource::isLooping() const { return false; }

void OffsetAudioSource::setLooping(bool shouldLoop) {}
