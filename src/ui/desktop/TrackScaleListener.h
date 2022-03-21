#pragma once

namespace trackman {

class TrackScaleListener {
  public:
    virtual void verticalScaleIncreased() = 0;
    virtual void verticalScaleDecreased() = 0;
    virtual void horizontalScaleIncreased() = 0;
    virtual void horizontalScaleDecreased() = 0;
};

} // namespace trackman
