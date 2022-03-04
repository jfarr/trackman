#pragma once

#include <JuceHeader.h>

#include "model/Track.h"

class TrackListListener {
  public:
    virtual void trackAdded(Track &track) {}
    virtual void selectionChanged(Track &track, juce::Component *source) {}
};
