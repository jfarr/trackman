#pragma once

#include <JuceHeader.h>

class ProcessingAudioSource : public juce::PositionableAudioSource {
  public:
    ProcessingAudioSource(PositionableAudioSource& source);
    ~ProcessingAudioSource();

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &info) override;

    //==============================================================================
    // PositionableAudioSource
    virtual void setNextReadPosition(juce::int64 newPosition) override;
    virtual juce::int64 getNextReadPosition() const override;
    virtual juce::int64 getTotalLength() const override;
    virtual bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    PositionableAudioSource& source;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessingAudioSource)
};
