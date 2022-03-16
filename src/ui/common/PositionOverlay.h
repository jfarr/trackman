#pragma once

#include <JuceHeader.h>
#include "model/Project.h"

class PositionOverlay : public juce::Component, private juce::Timer {
  public:
    explicit PositionOverlay(juce::AudioTransportSource &transportSource);

    void paint(juce::Graphics &g) override;

  private:
    void timerCallback() override;

    juce::AudioTransportSource &transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};
