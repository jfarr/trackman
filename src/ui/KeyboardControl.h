#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class KeyboardControl : public Component {
  public:
    explicit KeyboardControl(MidiKeyboardState &keyboardState);
    ~KeyboardControl() override = default;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardControl)
};

} // namespace trackman
