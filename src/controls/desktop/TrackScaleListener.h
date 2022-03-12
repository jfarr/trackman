#pragma once

class TrackScaleListener {
  public:
    virtual void scaleIncreased() = 0;
    virtual void scaleDecreased() = 0;
};
