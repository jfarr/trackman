#pragma once

#include <JuceHeader.h>
#include "audio/SynthAudioSource.h"

class Instrument {
  public:
    Instrument(juce::MidiKeyboardState &keyState) : synth(keyState) {}
    ~Instrument() = default;

  private:
    SynthAudioSource synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
