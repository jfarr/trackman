#pragma once

#include <JuceHeader.h>

#include "common/FileChooserControl.h"
#include "common/PositionOverlay.h"
#include "common/ThumbnailComponent.h"
#include "common/TransportControl.h"

using namespace std;
using namespace juce;

namespace trackman {

class AudioPlayer : public AudioAppComponent, public FileListener {
  public:
    AudioPlayer(AudioFormatManager &formatManager);
    ~AudioPlayer() override;

    void loopingChanged(bool shouldLoop);

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &event) override;

    //==============================================================================
    // FileListener
    void fileChosen(File file) override;

  private:
    AudioFormatManager &formatManager;
    AudioTransportSource transportSource;
    trackman::TransportControl transportControl;
    FileChooserControl fileChooserControl;
    ThumbnailComponent thumbnailComponent;
    PositionOverlay positionOverlay;
    unique_ptr<AudioFormatReaderSource> readerSource;

    //==============================================================================

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPlayer)
};

} // namespace trackman
