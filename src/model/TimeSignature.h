#pragma once

#include <JuceHeader.h>

class TimeSignature {
  public:
    TimeSignature() = default;
    ~TimeSignature() = default;

    TimeSignature(int numerator, int denominator) : numerator(numerator), denominator(denominator) {}
    TimeSignature(const TimeSignature &other) : numerator(other.numerator), denominator(other.denominator) {}

    int getNumerator() const { return numerator; }
    int getDenominator() const { return denominator; }

    double measuresToSeconds(double measures, int tempo) const {
        DBG("measuresToSeconds(" << measures << "," << tempo <<"): " << (double)(measures * numerator) / (double)tempo * 60.0);
        return (double)(measures * numerator) / (double)tempo * 60.0;
    }

  private:
    int numerator = 4;
    int denominator = 4;
};
