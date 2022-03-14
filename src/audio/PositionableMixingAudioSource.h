#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

class PositionableMixingAudioSource : public juce::PositionableAudioSource {
  public:
    PositionableMixingAudioSource(juce::AudioDeviceManager &deviceManager) : deviceManager(deviceManager) {}
    ~PositionableMixingAudioSource() override { removeAllInputs(); }

    void addInputSource(PositionableAudioSource *input);
    void removeInputSource(PositionableAudioSource *input);
    void removeAllInputs();

    void setTotalLengthSecs(double newLen);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double newSampleRate) override;
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
    juce::AudioDeviceManager &deviceManager;
    juce::MixerAudioSource mixer;
    juce::Array<PositionableAudioSource *> inputs;
    bool looping = false;
    double sampleRate = 0;
    juce::uint64 totalLength = 0;
    double totalLengthSecs = 0;

    juce::CriticalSection mutex;

    double getSampleRate();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionableMixingAudioSource)
};
