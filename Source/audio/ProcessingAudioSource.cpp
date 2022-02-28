#include "ProcessingAudioSource.h"

ProcessingAudioSource::ProcessingAudioSource(PositionableAudioSource &source) : source(source) {}

ProcessingAudioSource::~ProcessingAudioSource() {}

//==============================================================================
void ProcessingAudioSource::prepareToPlay(int samplesPerBlockExpected, double newSampleRate) {}

void ProcessingAudioSource::releaseResources() {}

void ProcessingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &info) {}

//==============================================================================
void ProcessingAudioSource::setNextReadPosition(juce::int64 newPosition) { source.setNextReadPosition(newPosition); }

juce::int64 ProcessingAudioSource::getNextReadPosition() const { return source.getNextReadPosition(); }

juce::int64 ProcessingAudioSource::getTotalLength() const { return source.getTotalLength(); }

bool ProcessingAudioSource::isLooping() const { return source.isLooping(); }

void ProcessingAudioSource::setLooping(bool shouldLoop) { source.setLooping(shouldLoop); }
