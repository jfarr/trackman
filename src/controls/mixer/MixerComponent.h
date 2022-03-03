#pragma once

#include "JuceHeader.h"

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "audio/PositionableMixingAudioSource.h"
#include "controls/common/FileChooserControl.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackController.h"
#include "controls/desktop/TrackSourceListener.h"
#include "model/Track.h"
#include "model/TrackList.h"

class MixerComponentListener {
  public:
    virtual void mixerResized(juce::Rectangle<int> area) = 0;
};

class MixerComponent : public juce::AudioAppComponent,
                       public TrackSourceListener,
                       public TransportControlListener,
                       public MasterTrackListener {
  public:
    MixerComponent();
    ~MixerComponent();

    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    void setLevel(float newLevel);

    void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

    void addListener(MixerComponentListener *listener);
    void removeListener(MixerComponentListener *listener);

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void levelChanged(float level) override;
    void muteToggled() override;

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
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<AudioSource *> sources;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    std::list<MixerComponentListener *> listeners;

    void createControls();

    void notifyResized(juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
