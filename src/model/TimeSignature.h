#pragma once

#include <JuceHeader.h>

class TimeSignature {
  public:
    TimeSignature() = default;
    ~TimeSignature() = default;

    TimeSignature(int numerator, int denominator) : numerator(numerator), denominator(denominator) {}
    TimeSignature(const TimeSignature &other) : numerator(other.numerator), denominator(other.denominator) {}

  private:
    int numerator = 4;
    int denominator = 4;
};
