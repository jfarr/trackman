#pragma once

#include <JuceHeader.h>

#include "TransportControl.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class AudioPlayer : public juce::AudioAppComponent, public TransportControlListener
{
public:
    //==============================================================================
    AudioPlayer();
    ~AudioPlayer() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // TransportControlListener
    void updateLoopState(bool shouldLoop) override;

private:
    juce::AudioTransportSource transportSource;
    std::unique_ptr<TransportControl> transportControl;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    //==============================================================================

    void openButtonClicked();

    std::unique_ptr<juce::FileChooser> chooser;

    juce::TextButton openButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayer)
};
