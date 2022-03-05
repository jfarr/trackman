#pragma once

#include <JuceHeader.h>

#include "controls/common/FileChooserControl.h"
#include "controls/common/PositionOverlay.h"
#include "controls/common/ThumbnailComponent.h"
#include "controls/common/TransportControl.h"

class AudioPlayer : public juce::AudioAppComponent, public FileListener, public TransportControlListener {
  public:
    //==============================================================================
    AudioPlayer(juce::AudioFormatManager &formatManager);
    ~AudioPlayer() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

  private:
    juce::AudioFormatManager &formatManager;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    FileChooserControl fileChooserControl;
    ThumbnailComponent thumbnailComponent;
    PositionOverlay positionOverlay;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    //==============================================================================

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayer)
};
