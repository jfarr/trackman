#pragma once

#include <JuceHeader.h>

class InstrumentsPanel : public juce::Component {
  public:
    InstrumentsPanel() = default;
    ~InstrumentsPanel() override = default;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentsPanel)
};
