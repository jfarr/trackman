#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Track;
class NoteRoll;

class Instrument {
  public:
    Instrument(Track &track);
    ~Instrument() = default;

    void renderNextBlock(
        AudioBuffer<float> &outputAudio, const MidiBuffer &inputMidi, int startSample, int numSamples);

  private:
    const int polyphony = 4;

    Track &track;
    Synthesiser liveSynth;
    Synthesiser playbackSynth;

    void initSynth(Synthesiser &synth);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};

} // namespace trackman
