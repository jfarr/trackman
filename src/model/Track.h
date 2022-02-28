#pragma once

#include "controls/mixer/TrackControl.h"
#include "controls/mixer/TrackLaneControl.h"
#include "audio/ProcessingAudioSource.h"

class GainProcessor : public juce::dsp::ProcessorWrapper<juce::dsp::Gain<float>> {};

class Track {
  public:
    Track(juce::String name);
    ~Track();

    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void setSource(juce::PositionableAudioSource *newSource, const bool deleteWhenRemoved);

  private:
    std::unique_ptr<ProcessingAudioSource<juce::dsp::Gain<float>>> source;
    GainProcessor gain;
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
