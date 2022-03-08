#pragma once

#include <JuceHeader.h>

#include "model/Track.h"

class TrackListListener {
  public:
    virtual void selectionChanged(Track *track) {}
};
