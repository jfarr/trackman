#pragma once

#include <JuceHeader.h>
#include "FileChooserControl.h"
#include "MasterTrackControl.h"
#include "PositionableMixingAudioSource.h"
#include "Track.h"
#include "TrackControl.h"
#include "TransportControl.h"


class MixerComponent : public juce::AudioAppComponent, public FileListener, public TransportControlListener
{
public:
    MixerComponent(juce::AudioFormatManager& formatManager);
    ~MixerComponent();

    void addTrack(Track& track);
    void fileChosen(juce::File file) override;
    void updateLoopState(bool shouldLoop);

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioFormatManager& formatManager;
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<Track*> tracks;
    std::list<AudioSource*> sources;

    void createControls();
};
