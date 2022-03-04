#pragma once

class TrackControl;

class TrackControlListener {
  public:
    virtual void levelChanged(float level) {}
    virtual void levelChangeFinalized(TrackControl &trackControl, float previousLevel) {}
    virtual void muteToggled() {}
};
