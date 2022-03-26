#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"

namespace trackman {

class Track;

class Instrument {
  public:
    Instrument(Track &track);
    ~Instrument() = default;

    //    Synthesiser &getSynth() { return synth; }

    void renderNextBlock(AudioBuffer<float> &outputAudio, const MidiBuffer &inputMidi, int startSample, int numSamples);

  private:
    const int polyphony = 4;

    Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};

} // namespace trackman
