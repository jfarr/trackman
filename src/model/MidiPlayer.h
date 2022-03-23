#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"

using namespace juce;

namespace trackman {

class Project;
class Track;

class SynthAudioSource : public PositionableAudioSource {
  public:
    SynthAudioSource(Track &track) : track(track) {
        for (auto i = 0; i < 4; ++i)
            synth.addVoice(new SineWaveVoice());

        synth.addSound(new SineWaveSound());
    }

    void setUsingSineWaveSound() { synth.clearSounds(); }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override {}
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    Track &track;
    Synthesiser synth;
    int64 currentPosition = 0;
    bool looping = false;
};
} // namespace trackman
