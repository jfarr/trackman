#pragma once

#include "JuceHeader.h"

#include "model/Project.h"
#include "model/Track.h"

using namespace juce;

namespace trackman {

class NoteRoll : public Component, public Timer {
  public:
    NoteRoll(Project &project, Track &track);
    ~NoteRoll() override = default;

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

    Rectangle<float> getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
        double noteStep, double x, double h, double scale, double margin);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

}
