#pragma once

#include "JuceHeader.h"

#include "audio/GainAudioSource.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/desktop/TrackSourceListener.h"
#include "controls/mixer/TrackControl.h"
#include "controls/tracks/TrackLaneControl.h"
#include "model/Track.h"

class TrackController : public FileListener, public TrackControlListener, public juce::MouseListener {
  public:
    TrackController(Track &track, TrackControl &trackControl, juce::AudioFormatManager &formatManager);
    ~TrackController();

    //    void update();
    void repaint();

    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    Track &getTrack() { return track; }
//    TrackControl &getTrackControl() { return trackControl; }

    //    void setSelected(bool newSelected);
    void setLevel(float newLevel);
    void setVisible(bool newVisible) { visible = newVisible; }
    bool isVisible() const { return visible; }

    void addSource();
    //    void removeSource();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TrackControlListener
    void levelChanged(float newLevel) override;
//    void levelChangeFinalized(TrackControl &trackControl, float previousLevel) override;
    void muteToggled() override;

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    juce::AudioFormatManager &formatManager;
    Track &track;
    std::shared_ptr<juce::PositionableAudioSource> gain;
    std::shared_ptr<juce::PositionableAudioSource> previousSource = nullptr;
    double sampleRate = 0;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    //    bool selected = false;
    bool visible = true;

    TrackControl &trackControl;

    TrackSourceListener *listener = nullptr;
    std::list<TrackListListener *> listeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
