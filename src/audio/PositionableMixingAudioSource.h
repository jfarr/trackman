#pragma once

#include "JuceHeader.h"

class PositionableMixingAudioSource : public juce::PositionableAudioSource {
  public:
    PositionableMixingAudioSource(double sampleRate);
    ~PositionableMixingAudioSource() override;

    void addInputSource(PositionableAudioSource *newInput, const bool deleteWhenRemoved,
        double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2);
    void removeInputSource(PositionableAudioSource *input);
    void removeAllInputs();

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &info) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    juce::MixerAudioSource mixer;
    juce::Array<PositionableAudioSource *> inputs;
    double sampleRate;
    bool looping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionableMixingAudioSource)
};
