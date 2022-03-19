#pragma once

#include <JuceHeader.h>
#include "audio/SynthAudioSource.h"

class Track;

class Instrument {
  public:
    Instrument(Track &track, juce::MidiKeyboardState &keyState) : synth(track) {}
    ~Instrument() = default;

  private:
    SynthAudioSource synth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Instrument)
};
