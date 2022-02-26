#include "ChildWindow.h"

ChildWindow::ChildWindow(const juce::String& name, juce::Component *component) : DocumentWindow(name, juce::Desktop::getInstance().getDefaultLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId), closeButton, false)
{
	setContentOwned(component, true);
}

ChildWindow::~ChildWindow()
{
}

void ChildWindow::closeButtonPressed()
{
    delete this;
}
