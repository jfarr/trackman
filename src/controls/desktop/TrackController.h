#pragma once

#include "JuceHeader.h"
#include "audio/GainAudioSource.h"
#include "controls/mixer/TrackControl.h"
#include "controls/tracks/TrackLaneControl.h"
#include "model/Track.h"

class TrackSourceListener {
  public:
    virtual void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) = 0;
};

class TrackController;

class TrackControllerListener {
  public:
    virtual void selectionChanged(TrackController *selected) = 0;
};

class TrackController : public FileListener, public TrackControlListener, public juce::MouseListener {
  public:
    TrackController(Track &track, juce::AudioFormatManager &formatManager);
    ~TrackController();

    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    Track &getTrack() { return track; }
    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void setSelected(bool newSelected);

    void addListener(TrackControllerListener *listener);
    void removeListener(TrackControllerListener *listener);

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TrackControlListener
    void levelChanged(float newLevel) override;
    void muteToggled() override;

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    juce::AudioFormatManager &formatManager;

    juce::PositionableAudioSource *source = nullptr;
    TrackSourceListener *listener = nullptr;
    std::list<TrackControllerListener *> listeners;

    Track &track;
    std::unique_ptr<juce::AudioSource> gain;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool selected = false;
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
