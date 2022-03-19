#pragma once

#include "JuceHeader.h"

#include "model/Project.h"
#include "model/Track.h"

class NoteRoll : public juce::Component, public juce::Timer {
  public:
    NoteRoll(Project &project, Track &track);
    ~NoteRoll() override = default;

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

    static juce::Rectangle<float> getNoteRect(const juce::MidiMessage &noteOn, const juce::MidiMessage &noteOff, int lowNote,
        double noteStep, double x, double h, double scale, double margin) ;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};
