#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

class PositionableMixingAudioSource : public juce::PositionableAudioSource {
  public:
    PositionableMixingAudioSource();
    ~PositionableMixingAudioSource() override;

    void addInputSource(PositionableAudioSource &input);
    void removeInputSource(PositionableAudioSource &input);
    void removeAllInputs();

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
    juce::MixerAudioSource mixer;
    juce::Array<PositionableAudioSource *> inputs;
    bool looping = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionableMixingAudioSource)
};
