#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

class MeteredAudioSource : public juce::AudioSource {
  public:
    MeteredAudioSource(double sampleRate);
    ~MeteredAudioSource();

    foleys::LevelMeterSource &getMeterSource() { return meterSource; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

  private:
    foleys::LevelMeterSource meterSource;
    double sampleRate;
    int blockSize = 512;
    int refreshRateHz = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeteredAudioSource)
};
