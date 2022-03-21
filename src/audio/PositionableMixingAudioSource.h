#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

using namespace juce;

namespace trackman {

class PositionableMixingAudioSource : public PositionableAudioSource {
  public:
    PositionableMixingAudioSource() = default;
    ~PositionableMixingAudioSource() override { removeAllInputs(); }

    void addInputSource(PositionableAudioSource *input);
    void removeInputSource(PositionableAudioSource *input);
    void removeAllInputs();

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double newSampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    MixerAudioSource mixer;
    Array<PositionableAudioSource *> inputs;
    bool looping = false;

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionableMixingAudioSource)
};

} // namespace trackman
