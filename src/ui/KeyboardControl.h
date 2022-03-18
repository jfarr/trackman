#pragma once

#include <JuceHeader.h>

class KeyboardControl : public juce::Component {
  public:
    KeyboardControl(juce::MidiKeyboardState &keyboardState);
    ~KeyboardControl() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardControl)
};
