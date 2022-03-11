#pragma once

#include "JuceHeader.h"

#include "audio/GainAudioSource.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/mixer/TrackControl.h"
#include "controls/tracks/TrackLaneControl.h"
#include "model/Track.h"

class DesktopController;

class TrackController : public TrackControlListener, public juce::MouseListener {
  public:
    TrackController(DesktopController &desktopController, Track &track, juce::AudioFormatManager &formatManager);
    ~TrackController();

    TrackControl &getTrackControl() { return trackControl; }
    Track &getTrack() const { return track; }

    void setLevel(float newLevel);
    void setMute(Track &track, bool newMute);
    void setSolo(Track &track, bool newSolo);
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // TrackControlListener
    void trackLevelChanged(Track &track, float newLevel) override;

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    DesktopController &desktopController;
    Track &track;
    juce::AudioFormatManager &formatManager;

    TrackControl trackControl;
    std::list<TrackListListener *> trackListListeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
