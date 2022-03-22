#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"

namespace trackman {

class Track;

class Instrument {
  public:
    Instrument(Track &track, MidiKeyboardState &keyState) : synth(track) {}
    ~Instrument() = default;

  private:
    SynthAudioSource synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};

} // namespace trackman
