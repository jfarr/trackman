#pragma once

#import "model/Track.h"

class TrackControlListener {
  public:
    virtual void trackNameChanged(Track &track, juce::String newName) {}
    virtual void trackLevelChanged(Track &track, float newLevel) {}
    virtual void trackLevelChangeFinalized(Track &track, float previousLevel) {}
    virtual void trackMuteToggled(Track &track) {}
    virtual void trackSoloToggled(Track &track) {}
};
