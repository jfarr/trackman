#pragma once
#include <JuceHeader.h>

#include "controls/desktop/DesktopComponent.h"

class MainWindow : public juce::DocumentWindow {
  public:
    MainWindow(juce::String name);

    void closeButtonPressed() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
