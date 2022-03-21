#pragma once

#include <JuceHeader.h>

#include "model/Track.h"

namespace trackman {

class TrackListListener {
  public:
    virtual void selectionChanged(Track *track) {}
};

} // namespace trackman
