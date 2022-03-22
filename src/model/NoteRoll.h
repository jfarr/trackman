#pragma once

#include <JuceHeader.h>

#include "Project.h"
#include "audio/SynthAudioSource.h"

using namespace juce;

namespace trackman {

class NoteRoll : public PositionableAudioSource {
  public:
    NoteRoll(Project &project, SynthAudioSource &synthAudioSource)
        : project(project), synthAudioSource(synthAudioSource) {}
    ~NoteRoll() = default;

  private:
    Project &project;
    SynthAudioSource &synthAudioSource;
    MidiMessageSequence midiMessages;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

} // namespace trackman
