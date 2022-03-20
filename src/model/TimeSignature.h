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

    double measuresToSeconds(double measures, float tempo) const {
        double beats = (measures - 1) * (double)numerator;
        double bps = (double)tempo / 60.0;
        double scale = 4.0 / (double)denominator;
        return beats / bps * scale;
    }

    double secondsToMeasures(double seconds, float tempo) const {
        double bps = (double)tempo / 60.0;
        double beats = seconds * bps;
        double measures = beats / (double)numerator;
        return measures + 1;
    }

  private:
    int numerator = 4;
    int denominator = 4;
};
