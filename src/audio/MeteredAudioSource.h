#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

#include "PositionableAudioSourceAdapter.h"

using namespace juce;
using namespace foleys;

namespace trackman {

class MeteredAudioSource : public PositionableAudioSourceAdapter {
  public:
    MeteredAudioSource(PositionableAudioSource *source, double sampleRate = 0);
    ~MeteredAudioSource();

    LevelMeterSource &getMeterSource() { return meterSource; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

  private:
    PositionableAudioSource *source;
    LevelMeterSource meterSource;
    double sampleRate;
    int blockSize = 512;
    int refreshRateHz = 30;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MeteredAudioSource)
};

} // namespace trackman
