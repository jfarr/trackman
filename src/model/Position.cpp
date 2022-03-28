#include "Position.h"

namespace trackman {

Position::Position(int measure, int beat, int fraction) : measure(measure), beat(beat), fraction(fraction) {}

Position::Position(const Position &other) : measure(other.measure), beat(other.beat), fraction(other.fraction) {}

} // namespace trackman
