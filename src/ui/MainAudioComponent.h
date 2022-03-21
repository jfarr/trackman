#pragma once

#include <JuceHeader.h>

#include "ui/desktop/DesktopController.h"

using namespace juce;

namespace trackman {

class MainWindow;

class MainAudioComponent : public AudioAppComponent {
  public:
    explicit MainAudioComponent(MainWindow &mainWindow);
    ~MainAudioComponent() override;

    AudioFormatManager &getFormatManager() { return formatManager; }
    AudioDeviceManager &getDeviceManager() { return deviceManager; }
    DesktopController &getDesktopController() { return desktopController; }

    //==============================================================================
    // Component
    void resized() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int blockSize, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

  private:
    const int topStripHeight = 15;

    AudioFormatManager formatManager;
    DesktopController desktopController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};

}
