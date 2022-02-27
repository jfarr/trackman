#pragma once

#include <JuceHeader.h>

class ChildWindow : public juce::DocumentWindow
{
public:
	ChildWindow(const juce::String& name, juce::Component *component);
	~ChildWindow() override;

	void closeButtonPressed() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChildWindow)
};
