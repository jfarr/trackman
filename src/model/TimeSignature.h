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
        double beats = measures * (double)numerator;
        double bps = (double)tempo * 60.0;
        double scale = 4.0 / (double)denominator;
        return beats / bps * scale;
    }

    double secondsToMeasures(double seconds, int tempo) const {
        double bps = (double)tempo * 60.0;
        double beats = seconds * bps;
        double scale = 4.0 / (double)denominator;
        return beats * (double)numerator * scale;
    }

  private:
    int numerator = 4;
    int denominator = 4;
};
