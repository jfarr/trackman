#pragma once

#include <JuceHeader.h>

class TrackController;

class TrackControllerListener {
  public:
    virtual void selectionChanged(TrackController *newSelected) {}
};
