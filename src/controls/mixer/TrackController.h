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

    Track &getTrack() const { return track; }

    void setLevel(float newLevel);
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);
    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

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
    Track &track;
    TrackControl &trackControl;
    juce::AudioFormatManager &formatManager;

    std::list<TrackListListener *> listeners;
    TrackSourceListener *listener = nullptr;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
