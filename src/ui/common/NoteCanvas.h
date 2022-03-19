#pragma once

#include <JuceHeader.h>

#include "model/Project.h"
#include "model/Track.h"

class NoteCanvas : public juce::Component, public juce::Timer {
  public:
    NoteCanvas(Project &project, Track &track);
    ~NoteCanvas() override = default;

    void resize();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;

    //==============================================================================
    // Timer
    void timerCallback() override { resize(); }

  private:
    Project &project;
    Track &track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteCanvas)
};
