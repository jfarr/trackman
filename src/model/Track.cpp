#include "Track.h"

Track::Track(juce::String name) : name(name) {}

Track::~Track() {}

void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate) {
    source = newSource;
    sampleRate = newSampleRate;
}
