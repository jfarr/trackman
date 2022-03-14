#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

#include "PositionableAudioSourceAdapter.h"

class MeteredAudioSource : public PositionableAudioSourceAdapter {
  public:
    MeteredAudioSource(juce::PositionableAudioSource *source, double sampleRate = 0);
    ~MeteredAudioSource();

    foleys::LevelMeterSource &getMeterSource() { return meterSource; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

  private:
    juce::PositionableAudioSource *source;
    foleys::LevelMeterSource meterSource;
    double sampleRate;
    int blockSize = 512;
    int refreshRateHz = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeteredAudioSource)
};
