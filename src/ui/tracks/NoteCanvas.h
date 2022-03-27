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

    function<void(Track &, NoteRoll &)> onSelected = nullptr;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void mouseDown(const MouseEvent &event) override;
    void mouseUp(const MouseEvent &event) override;
    void mouseDrag(const MouseEvent &event) override;

    //==============================================================================
    // Timer
    void timerCallback() override {
        if (noteRoll.isRecording()) {
            resize();
        }
    }

  private:
    friend class TrackLaneControl;

    Project &project;
    Track &track;
    NoteRoll &noteRoll;

    Rectangle<float> getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote, double noteStep,
        double x, double h, double scale);
    void notifySelected(Track &track, NoteRoll &selected);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteCanvas)
};

} // namespace trackman
