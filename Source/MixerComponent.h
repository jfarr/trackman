#pragma once

#include <JuceHeader.h>

#include "FileChooserControl.h"
#include "MasterTrackControl.h"
#include "PositionableMixingAudioSource.h"
#include "Track.h"
#include "TrackControl.h"
#include "TransportControl.h"

class MixerComponent : public juce::AudioAppComponent,
                       public FileListener,
                       public TransportControlListener,
                       public MasterTrackListener {
   public:
    MixerComponent(juce::AudioFormatManager &formatManager);
    ~MixerComponent();

    void addTrack(Track &track);

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TransportControlListener
    void updateLoopState(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void levelChanged(float level) override;
    void muteChanged(bool muted) override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(
        const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

   private:
    juce::AudioFormatManager &formatManager;
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<Track *> tracks;
    std::list<AudioSource *> sources;

    void createControls();
};
