#pragma once

#include <JuceHeader.h>

class InstrumentControl : public juce::Component {
  public:
    InstrumentControl() = default;
    ~InstrumentControl() override = default;

    int getPreferredHeight() const { return preferredHeight; }

    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int preferredHeight = 75;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentControl)
};
