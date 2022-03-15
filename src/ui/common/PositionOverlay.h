#pragma once

#include <JuceHeader.h>
#include "model/Project.h"

class PositionOverlay : public juce::Component, private juce::Timer {
  public:
    PositionOverlay(juce::AudioTransportSource &transportSource, Project *project = nullptr);

    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    void timerCallback() override;

    juce::AudioTransportSource &transportSource;
    Project *project;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};
