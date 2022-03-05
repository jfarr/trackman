#pragma once

#include <JuceHeader.h>

class OffsetAudioSource : public juce::PositionableAudioSource {
  public:
    OffsetAudioSource(PositionableAudioSource &source, juce::int64 offsetSeconds);
    ~OffsetAudioSource();

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    PositionableAudioSource &source;
    juce::int64 offsetSeconds;
};
