#pragma once

#include <JuceHeader.h>

class MainAudioComponent : public juce::AudioAppComponent {
  public:
    MainAudioComponent();
    ~MainAudioComponent();

        //==============================================================================
        // AudioAppComponent
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
        void releaseResources() override;

  private:
    juce::AudioFormatManager formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainAudioComponent)
};
