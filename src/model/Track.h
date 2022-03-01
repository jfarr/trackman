#pragma once

#include "audio/GainAudioSource.h"
#include "controls/mixer/TrackControl.h"
#include "controls/mixer/TrackLaneControl.h"

class TrackSourceListener {
  public:
    virtual void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) = 0;
};

class Track : public FileListener, public TrackControlListener {
  public:
    Track(juce::String name, juce::AudioFormatManager &formatManager);
    ~Track();

    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TrackControlListener
    void levelChanged(float level) override;
    void muteChanged(bool muted) override;

  private:
    juce::AudioFormatManager &formatManager;

    juce::PositionableAudioSource *source = nullptr;
    TrackSourceListener *listener = nullptr;

    std::unique_ptr<juce::AudioSource> gain;
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
