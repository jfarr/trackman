#pragma once

#import "model/Track.h"

class TrackControlListener {
  public:
    virtual void nameChanged(Track &track, juce::String newName) {}
    virtual void levelChanged(Track &track, float level) {}
    virtual void levelChangeFinalized(Track &track, float previousLevel) {}
    virtual void muteToggled(Track &track) {}
    virtual void soloToggled(Track &track) {}
};
