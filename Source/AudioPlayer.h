#pragma once

#include <JuceHeader.h>

#include "FileChooserControl.h"
#include "TransportControl.h"
#include "ThumbnailComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class AudioPlayer : public juce::AudioAppComponent, public FileListener, public TransportControlListener
{
public:
    //==============================================================================
    AudioPlayer(juce::AudioFormatManager& formatManager);
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

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file);

private:
    juce::AudioFormatManager& formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<TransportControl> transportControl;
    std::unique_ptr<FileChooserControl> fileChooserControl;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    ThumbnailComponent thumbnailComponent;
    PositionOverlay positionOverlay;
 
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPlayer)
};
