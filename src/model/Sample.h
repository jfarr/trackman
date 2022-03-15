#pragma once

#include <JuceHeader.h>
#include <utility>

#include "audio/PositionableResamplingAudioSource.h"

class Track;

class Sample : public juce::PositionableAudioSource {
  public:
    Sample(juce::File file, double startPos, double endPos);
    ~Sample() override;

    [[nodiscard]] juce::File getFile() const { return file; }
    [[nodiscard]] double getStartPos() const { return startPos; }
    [[nodiscard]] double getEndPos() const { return endPos; }
    [[nodiscard]] juce::int64 getLengthInSamples() const;
    [[nodiscard]] double getLengthInSeconds() const { return length; }
    [[nodiscard]] bool isLoaded() const { return loaded; }
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    void loadFile(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);

    void setPosition(double pos);
    void setLength(double newLength);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    [[nodiscard]] juce::int64 getNextReadPosition() const override;
    [[nodiscard]] juce::int64 getTotalLength() const override;
    [[nodiscard]] bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    juce::File file;
    double startPos;
    double endPos;
    double length;
    double sourceLengthInSeconds = 0;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    std::unique_ptr<PositionableResamplingAudioSource> resamplingSource;
    bool loaded = false;
    bool selected = false;
    bool deleted = false;

    [[nodiscard]] juce::int64 getPositionFromTime(double t) const;
    [[nodiscard]] double getSampleRate() const;

    std::unique_ptr<juce::AudioFormatReader> reader;
};
