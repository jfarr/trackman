#pragma once
#include <JuceHeader.h>

#include "MainAudioComponent.h"
#include "controls/desktop/DesktopComponent.h"

class MainWindow : public juce::DocumentWindow {
  public:
    MainWindow(juce::String name);

    MainAudioComponent &getMainAudioComponent() { return mainAudioComponent; }

    void closeButtonPressed() override;

  private:
    MainAudioComponent &mainAudioComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
