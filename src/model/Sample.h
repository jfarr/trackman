#pragma once

#include "audio/OffsetAudioSource.h"
#include <JuceHeader.h>

class Sample {
  public:
    Sample(juce::File file, double startPos, double endPos, double sourceLengthSecs, double sampleRate)
        : file(file), startPos(startPos), endPos(endPos), length(sourceLengthSecs),
          sourceLengthSecs(sourceLengthSecs), sourceSampleRate(sampleRate) {}
    ~Sample() {}

    juce::File getFile() const { return file; }
    double getStartPos() const { return startPos; }
    double getEndPos() const { return endPos; }
    double getSourceLengthSecs() const { return sourceLengthSecs; }
    double getSampleRate() const { return sourceSampleRate; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioFormatManager &formatManager);
    juce::PositionableAudioSource *getSource() { return offsetSource.get(); }

    void setMinLengthSecs(double newLength);
    void setPosition(double pos);
    void setLength(double length);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    juce::File file;
    double startPos;
    double endPos;
    double length;
    double sourceLengthSecs;
    double sourceSampleRate;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    std::unique_ptr<OffsetAudioSource> offsetSource;
    bool selected = false;
    bool deleted = false;
};
