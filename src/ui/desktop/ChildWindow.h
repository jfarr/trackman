#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class ChildWindow : public DocumentWindow {
  public:
    ChildWindow(const String &name, Component *component);
    ~ChildWindow() override = default;

    void closeButtonPressed() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChildWindow)
};

} // namespace trackman
