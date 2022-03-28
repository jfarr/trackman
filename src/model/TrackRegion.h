#pragma once

namespace trackman {

class TrackRegion {
  public:
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

    virtual double getStartPosInSeconds() const = 0;
    virtual double getEndPosInSeconds() const = 0;

  private:
    bool selected = false;
    bool deleted = false;
};

} // namespace trackman
