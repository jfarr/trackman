#pragma once

#include "MeteredAudioSource.h"
#include "PositionableMixingAudioSource.h"

class MixerAudioSource : public PositionableMixingAudioSource, public MeteredAudioSource {
  public:
    MixerAudioSource(double sampleRate) : PositionableMixingAudioSource(sampleRate), MeteredAudioSource(sampleRate) {}
    ~MixerAudioSource() {}

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override {
        PositionableMixingAudioSource::prepareToPlay(blockSize, sampleRate);
        MeteredAudioSource::prepareToPlay(blockSize, sampleRate);
    }

    void releaseResources() override {
        MeteredAudioSource::releaseResources();
        PositionableMixingAudioSource::releaseResources();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        PositionableMixingAudioSource::getNextAudioBlock(bufferToFill);
        MeteredAudioSource::getNextAudioBlock(bufferToFill);
    }

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerAudioSource)
};
