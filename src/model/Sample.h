#pragma once

#include "audio/OffsetAudioSource.h"
#include "audio/PositionableResamplingAudioSource.h"
#include <JuceHeader.h>

#include <utility>

class Track;

class Sample {
  public:
    Sample(Track &track, juce::File file, double startPos, double endPos, double sourceLengthSecs, double sampleRate)
        : track(&track), file(std::move(file)), startPos(startPos), endPos(endPos), length(sourceLengthSecs),
          sourceLengthSecs(sourceLengthSecs), sourceSampleRate(sampleRate) {}
    ~Sample();

    [[nodiscard]] juce::File getFile() const { return file; }
    [[nodiscard]] double getStartPos() const { return startPos; }
    [[nodiscard]] double getEndPos() const { return endPos; }
    [[nodiscard]] double getLengthSecs() const { return length; }
    [[nodiscard]] double getSourceLengthSecs() const { return sourceLengthSecs; }
    [[nodiscard]] double getSampleRate() const { return sourceSampleRate; }
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioFormatManager &formatManager, double sampleRate);
    juce::PositionableAudioSource *getSource() { return offsetSource; }

    void setPosition(double pos);
    void setLength(double newLength);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    friend Track;

    Track *track;
    juce::File file;

    void setTrack(Track &newTrack);

    double startPos;
    double endPos;
    double length;
    double sourceLengthSecs;
    double sourceSampleRate;
    juce::AudioFormatReaderSource *fileSource = nullptr;
    PositionableResamplingAudioSource *resamplingSource = nullptr;
    OffsetAudioSource *offsetSource = nullptr;
    bool selected = false;
    bool deleted = false;
};
