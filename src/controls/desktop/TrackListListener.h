#pragma once

#include "model/Track.h"

class TrackListListener {
  public:
    virtual void trackAdded(Track &track) = 0;
};
