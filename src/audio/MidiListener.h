#pragma once

#include <JuceHeader.h>

class MidiListener {
  public:
    virtual void midiMessageReceived(const juce::MidiMessage &message) = 0;
};
