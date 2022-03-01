#pragma once

#include "audio/GainAudioSource.h"
#include "controls/mixer/TrackControl.h"
#include "controls/tracks/TrackLaneControl.h"
#include "model/Track.h"

class TrackSourceListener {
  public:
    virtual void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) = 0;
};

class TrackController : public FileListener, public TrackControlListener {
  public:
    TrackController(Track& track, juce::AudioFormatManager &formatManager);
    ~TrackController();

    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TrackControlListener
    void levelChanged(float newLevel) override;
    void muteToggled() override;

  private:
    juce::AudioFormatManager &formatManager;

    juce::PositionableAudioSource *source = nullptr;
    TrackSourceListener *listener = nullptr;

    Track& track;
    std::unique_ptr<juce::AudioSource> gain;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
