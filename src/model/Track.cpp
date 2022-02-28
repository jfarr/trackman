#include "Track.h"


Track::Track(juce::String name) : trackControl(name) {}

Track::~Track() {}

void Track::setSource(juce::PositionableAudioSource *newSource, const bool deleteWhenRemoved) {
    source.reset(new ProcessingAudioSource<juce::dsp::Gain<float>>(newSource, &gain, true));
}
