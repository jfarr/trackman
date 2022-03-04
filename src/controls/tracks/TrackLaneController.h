#pragma once

#include <JuceHeader.h>
#include "model/Track.h"
#include "TrackLaneControl.h"

class TrackLaneController {
  public:
    TrackLaneController(Track &track);
    ~TrackLaneController() {}

    TrackLaneControl& getTrackLaneControl() { return trackLaneControl; }

  private:
    TrackLaneControl trackLaneControl;
};
