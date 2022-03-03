#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanelListener.h"
#include "TrackControl.h"
#include "audio/PositionableMixingAudioSource.h"
#include "controls/common/FileChooserControl.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackController.h"
#include "controls/desktop/TrackSourceListener.h"
#include "model/Track.h"
#include "model/TrackList.h"

class MixerPanel : public juce::AudioAppComponent,
                       public TrackSourceListener,
                       public TransportControlListener,
                       public MasterTrackListener {
  public:
    MixerPanel();
    ~MixerPanel();

    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    void setMasterLevel(float newLevel);
    void setTrackLevel(Track *track, float newLevel);

    void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

    void addListener(MixerPanelListener *listener);
    void removeListener(MixerPanelListener *listener);

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;
    void masterMuteToggled() override;

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
    // TODO: factor out MixerController
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<AudioSource *> sources;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    std::list<MixerPanelListener *> listeners;

    void createControls();

    void notifyResized(juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
