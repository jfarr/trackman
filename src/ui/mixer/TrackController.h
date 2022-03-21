#pragma once

#include <JuceHeader.h>

#include "audio/GainAudioSource.h"
#include "model/Track.h"
#include "ui/desktop/TrackListListener.h"
#include "ui/mixer/TrackControl.h"
#include "ui/tracks/TrackLaneControl.h"

using namespace std;
using namespace juce;

namespace trackman {

class DesktopController;

class TrackController : public TrackControlListener, public MouseListener {
  public:
    TrackController(DesktopController &desktopController, Track &track);
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
    void mouseDown(const MouseEvent &event) override;

  private:
    DesktopController &desktopController;
    Track &track;

    TrackControl trackControl;
    list<TrackListListener *> trackListListeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};

}
