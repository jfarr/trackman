#pragma once

#include <JuceHeader.h>

#include "PositionableAudioSourceAdapter.h"

using namespace juce;

namespace trackman {

class GainAudioSource : public PositionableAudioSourceAdapter {
  public:
    GainAudioSource(PositionableAudioSource *source, const bool deleteWhenRemoved);
    ~GainAudioSource() override;

    void setGain(float newGain) { gain = newGain; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

  private:
    CriticalSection callbackLock;
    float gain = 1.0f, lastGain = 1.0f;
};

} // namespace trackman
