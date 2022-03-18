#pragma once

#include <JuceHeader.h>

#include "model/Project.h"

class NoteCanvas : public juce::Component {
  public:
    NoteCanvas(Project &project, Track &track);
    ~NoteCanvas() override = default;

    void resize();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;

  private:
    Project &project;
    Track &track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteCanvas)
};
