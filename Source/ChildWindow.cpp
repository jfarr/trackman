#include "ChildWindow.h"

ChildWindow::ChildWindow(const juce::String &name, juce::Component *component)
    : DocumentWindow(
          name,
          juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(
              juce::ResizableWindow::backgroundColourId),
          closeButton, false) {
    juce::Rectangle<int> area(0, 0, 300, 200);
    setBounds(area);
    setResizable(true, false);
    setUsingNativeTitleBar(false);
    setContentOwned(component, true);
}

ChildWindow::~ChildWindow() {}

void ChildWindow::closeButtonPressed() { delete this; }
