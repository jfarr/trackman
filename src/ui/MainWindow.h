#pragma once
#include <JuceHeader.h>

#include "MainAudioComponent.h"
#include "ui/desktop/DesktopComponent.h"

using namespace juce;

namespace trackman {

class MainWindow : public DocumentWindow {
  public:
    MainWindow(String name);

    MainAudioComponent &getMainAudioComponent() { return mainAudioComponent; }

    void closeButtonPressed() override;

  private:
    MainAudioComponent &mainAudioComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

} // namespace trackman
