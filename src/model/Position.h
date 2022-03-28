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

    Position rounded() const { return {measure, beat, 0}; }

  private:
    const int measure;
    const int beat;
    const int fraction;
};

} // namespace trackman
