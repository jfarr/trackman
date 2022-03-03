#pragma once

#include <JuceHeader.h>

class TrackSourceListener {
  public:
    virtual void onSourceSet(std::shared_ptr<juce::PositionableAudioSource> newSource, std::shared_ptr<juce::PositionableAudioSource> prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) = 0;
};
