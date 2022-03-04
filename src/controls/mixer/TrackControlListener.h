#pragma once

#import "model/Track.h"

class TrackControlListener {
  public:
    virtual void levelChanged(float level) {}
    virtual void levelChangeFinalized(Track &track, float previousLevel) {}
    virtual void muteToggled() {}
};
