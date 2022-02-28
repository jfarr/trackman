#pragma once

#include "JuceHeader.h"

#include "controls/common/FileChooserControl.h"
#include "MasterTrackControl.h"
#include "audio/PositionableMixingAudioSource.h"
#include "model/Track.h"
#include "TrackControl.h"
#include "controls/common/TransportControl.h"

class MixerComponent : public juce::AudioAppComponent,
                       public TrackSourceListener,
                       public TransportControlListener,
                       public MasterTrackListener {
  public:
    MixerComponent(juce::AudioFormatManager &formatManager);
    ~MixerComponent();

    void addTrack(Track &track);

    void onSourceSet(juce::PositionableAudioSource *source, const bool deleteWhenRemoved,
        double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

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
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
