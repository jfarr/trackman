#pragma once
#include <JuceHeader.h>

#include "controls/desktop/DesktopComponent.h"

class MainWindow : public juce::DocumentWindow {
  public:
    MainWindow(juce::String name);

//    DesktopComponent *getDesktopComponent() { return desktop; }
//    DesktopController &getDesktopController() { return desktopController; }

    void closeButtonPressed() override;

  private:
//    DesktopComponent *desktop;
//    DesktopController desktopController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
