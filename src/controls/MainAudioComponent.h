#pragma once

#include <JuceHeader.h>

#include "controls/desktop/DesktopController.h"

class MainWindow;

class MainAudioComponent : public juce::AudioAppComponent {
  public:
    MainAudioComponent(MainWindow &mainWindow);
    ~MainAudioComponent();

    juce::AudioFormatManager &getFormatManager() { return formatManager; }
    juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }

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
    DesktopController desktopController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};
