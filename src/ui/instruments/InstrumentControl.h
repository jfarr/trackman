#pragma once

#include <JuceHeader.h>

class InstrumentControl : public juce::Component {
  public:
    InstrumentControl() = default;
    ~InstrumentControl() override = default;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentControl)
};
