#pragma once

#include <JuceHeader.h>
#include "audio/OffsetAudioSource.h"

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
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioFormatManager &formatManager);
    juce::PositionableAudioSource *getSource() { return offsetSource.get(); }

    void setPosition(double pos);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    juce::File file;
    double startPos;
    double endPos;
    double length;
    double sampleRate;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    std::unique_ptr<OffsetAudioSource> offsetSource;
    bool selected = false;
    bool deleted = false;
};
