#pragma once

#include <JuceHeader.h>

class ChildWindow : public juce::DocumentWindow
{
public:
	ChildWindow(const juce::String& name, juce::Component *component);
	~ChildWindow() override;

	void closeButtonPressed();
};
