#pragma once

#include <JuceHeader.h>

class TrackSourceListener {
  public:
    virtual void onSourceSet() = 0;
};
