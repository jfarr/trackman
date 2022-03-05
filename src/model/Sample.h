#pragma once

#include <JuceHeader.h>

class Sample {
  public:
    Sample(juce::File file, double startPos, double endPos, double length, double sampleRate)
        : file(file), startPos(startPos), endPos(endPos), length(length), sampleRate(sampleRate) {}
    ~Sample() {}

    juce::File getFile() const { return file; }
    double getStartPos() const { return startPos; }
    double getEndPos() const { return endPos; }
    double getLength() const { return length; }
    double getSampleRate() const { return sampleRate; }

  private:
    juce::File file;
    double startPos;
    double endPos;
    double length;
    double sampleRate;
};
