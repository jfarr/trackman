#pragma once

namespace trackman {

class Position {
  public:
    Position(int measure, int beat, int fraction = 0);
    Position(const Position &other);
    ~Position() = default;

    int getMeasure() const { return measure; }
    int getBeat() const { return beat; }
    int getFraction() const { return fraction; }

  private:
    int measure;
    int beat;
    int fraction;
};

} // namespace trackman
