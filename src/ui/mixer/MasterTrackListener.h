#pragma once

class MasterTrackListener {
  public:
    virtual void masterLevelChanged(float level) {}
    virtual void masterLevelChangeFinalized(float previousLevel) {}
    virtual void masterMuteToggled() {}
};