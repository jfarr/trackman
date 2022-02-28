#pragma once

#include "controls/mixer/TrackControl.h"
#include "controls/mixer/TrackLaneControl.h"

class Track {
  public:
    Track(juce::String name);
    ~Track();

    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

  private:
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
