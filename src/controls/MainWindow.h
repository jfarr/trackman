#pragma once
#include <JuceHeader.h>

#include "controls/desktop/DesktopComponent.h"

class MainWindow : public juce::DocumentWindow {
  public:
    MainWindow(juce::String name, juce::AudioFormatManager &formatManager);

    DesktopComponent *getDesktopComponent() { return desktop; }

    void closeButtonPressed() override;

  private:
    DesktopComponent *desktop;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
