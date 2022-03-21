#pragma once

#include <JuceHeader.h>

#include "ui/desktop/DesktopController.h"

class MainWindow;

class MainAudioComponent : public juce::AudioAppComponent {
  public:
    explicit MainAudioComponent(MainWindow &mainWindow);
    ~MainAudioComponent() override;

    juce::AudioFormatManager &getFormatManager() { return formatManager; }
    juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }
    trackman::DesktopController &getDesktopController() { return desktopController; }

    //==============================================================================
    // Component
    void resized() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int blockSize, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

  private:
    const int topStripHeight = 15;

    juce::AudioFormatManager formatManager;
    trackman::DesktopController desktopController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};
