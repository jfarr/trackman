#pragma once

#include "audio/OffsetAudioSource.h"
#include "audio/PositionableResamplingAudioSource.h"
#include <JuceHeader.h>

#include <utility>

class Sample {
  public:
    Sample(juce::File file, double startPos, double endPos, double sourceLengthSecs, double sampleRate)
        : file(std::move(file)), startPos(startPos), endPos(endPos), length(sourceLengthSecs),
          sourceLengthSecs(sourceLengthSecs), sourceSampleRate(sampleRate) {}
    ~Sample() = default;

    [[nodiscard]] juce::File getFile() const { return file; }
    [[nodiscard]] double getStartPos() const { return startPos; }
    [[nodiscard]] double getEndPos() const { return endPos; }
    [[nodiscard]] double getLengthSecs() const { return length; }
    [[nodiscard]] double getSourceLengthSecs() const { return sourceLengthSecs; }
    [[nodiscard]] double getSampleRate() const { return sourceSampleRate; }
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioFormatManager &formatManager, double sampleRate);
    juce::PositionableAudioSource *getSource() { return offsetSource.get(); }

    void setMinLengthSecs(double newLength);
    void setPosition(double pos);
    void setLength(double newLength);
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
    std::unique_ptr<PositionableResamplingAudioSource> resamplingSource;
    std::unique_ptr<OffsetAudioSource> offsetSource;
    bool selected = false;
    bool deleted = false;
};
