#pragma once

#include <JuceHeader.h>

class PositionOverlay : public juce::Component, private juce::Timer {
  public:
    PositionOverlay(juce::AudioTransportSource &transportSource);

    void paint(juce::Graphics &g) override;
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    void timerCallback() override;

    juce::AudioTransportSource &transportSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};
