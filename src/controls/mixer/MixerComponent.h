#pragma once

#include "JuceHeader.h"

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "audio/PositionableMixingAudioSource.h"
#include "controls/common/FileChooserControl.h"
#include "controls/common/TransportControl.h"
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

    void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

    void addListener(MixerComponentListener* listener);
    void removeListener(MixerComponentListener* listener);

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
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<AudioSource *> sources;
    std::list<MixerComponentListener *> listeners;

    void createControls();

    void notifyResized(juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerComponent)
};
