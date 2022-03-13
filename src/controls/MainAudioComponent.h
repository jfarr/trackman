#pragma once

#include <JuceHeader.h>

#include "controls/desktop/DesktopController.h"

class MainAudioComponent : public juce::AudioAppComponent {
  public:
    MainAudioComponent(juce::DocumentWindow &mainWindow);
    ~MainAudioComponent();

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

  private:
//    juce::DocumentWindow &mainWindow;
    juce::AudioFormatManager formatManager;
    DesktopController desktopController;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};
