#pragma once

#include <JuceHeader.h>

#include "model/Project.h"
#include "model/Track.h"

using namespace juce;

namespace trackman {

class NoteCanvas : public Component, public Timer {
  public:
    NoteCanvas(Project &project, Track &track, NoteRoll &noteRoll);
    ~NoteCanvas() override = default;

    NoteRoll &getNoteRoll() { return noteRoll; }

    void resize();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;

    //==============================================================================
    // Timer
    void timerCallback() override { resize(); }

  private:
    Project &project;
    Track &track;
    NoteRoll &noteRoll;

    Rectangle<float> getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
        double noteStep, double x, double h, double scale, double margin);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteCanvas)
};

}
