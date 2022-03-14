#pragma once

#include "audio/OffsetAudioSource.h"
#include "audio/PositionableResamplingAudioSource.h"
#include <JuceHeader.h>

#include <utility>

class Track;

class Sample {
  public:
    Sample(juce::File file, double startPos, double endPos);
    ~Sample();

    [[nodiscard]] juce::File getFile() const { return file; }
    [[nodiscard]] double getStartPos() const { return startPos; }
    [[nodiscard]] double getEndPos() const { return endPos; }
    [[nodiscard]] juce::int64 getLengthInSamples() const;
    [[nodiscard]] double getLengthInSeconds() const { return length; }
    //    [[nodiscard]] double getSourceLengthInSeconds() const { return sourceLengthInSeconds; }
    [[nodiscard]] double getSampleRate() const { return sourceSampleRate; }
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioFormatManager &formatManager, double sampleRate);
    juce::PositionableAudioSource *getSource() { return fileSource.get(); }

    void setPosition(double pos);
    void setLength(double newLength);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    //    friend Track;

    //    Track *track;
    juce::File file;

    //    void setTrack(Track &newTrack);

    double startPos;
    double endPos;
    double length;
    juce::int64 sourceLengthInSamples;
    double sourceLengthInSeconds;
    double sourceSampleRate;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    //    std::unique_ptr<PositionableResamplingAudioSource> resamplingSource;
    //    std::unique_ptr<OffsetAudioSource> offsetSource;
    bool selected = false;
    bool deleted = false;

    std::unique_ptr<juce::AudioFormatReader> reader;
};
