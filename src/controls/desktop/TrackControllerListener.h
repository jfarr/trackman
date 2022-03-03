#pragma once

class TrackController;

class TrackControllerListener {
  public:
    virtual void selectionChanged(TrackController *selected) {}
};
