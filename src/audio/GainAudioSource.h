#pragma once

#include "JuceHeader.h"
#include "PositionableAudioSourceAdapter.h"

class GainAudioSource : public PositionableAudioSourceAdapter {
  public:
    GainAudioSource(PositionableAudioSource *source, const bool deleteWhenRemoved);
    ~GainAudioSource() override;

    void setGain(float newGain) { gain = newGain; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

  private:
    juce::CriticalSection callbackLock;
    float gain = 1.0f, lastGain = 1.0f;
};
