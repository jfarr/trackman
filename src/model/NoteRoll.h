#pragma once

#include <JuceHeader.h>

#include "Project.h"
#include "audio/SynthAudioSource.h"

using namespace juce;

namespace trackman {

class NoteRoll {
  public:
    NoteRoll(const MidiMessageSequence &midiMessages, int startTick, int endTick)
        : midiMessages(midiMessages), startTick(startTick), endTick(endTick) {}
    ~NoteRoll() = default;

  private:
    MidiMessageSequence midiMessages;
    int startTick;
    int endTick;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

} // namespace trackman
