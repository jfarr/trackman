#include "Track.h"

Track::Track(juce::String name) : name(name) {}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate) {
    if (source != nullptr) {
        source->releaseResources();
    }
    source = newSource;
    sampleRate = newSampleRate;
    gain = std::shared_ptr<GainAudioSource>(new GainAudioSource(newSource.get(), false));
}

void Track::setLevel(float newLevel) {
    level = newLevel;
    if (gain != nullptr) {
        gain->setGain(level);
    }
}
