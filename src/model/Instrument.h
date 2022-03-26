#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"
#include "MidiPlayer.h"

namespace trackman {

class Track;

class Instrument {
  public:
    Instrument(Track &track, MidiKeyboardState &keyState) {}
    ~Instrument() = default;

    Synthesiser &getSynth() { return synth; }

  private:
    Synthesiser synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};

} // namespace trackman
