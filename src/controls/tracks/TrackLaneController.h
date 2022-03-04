#pragma once

#include <JuceHeader.h>

#include "TrackLaneControl.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Track.h"

class TrackLaneController : public juce::MouseListener, public TrackListListener {
  public:
    TrackLaneController(Track &track);
    ~TrackLaneController() {}

    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track, juce::Component *source) override;

  private:
    Track &track;
    TrackLaneControl trackLaneControl;
    std::list<TrackListListener *> listeners;

    void notifySelectionChanged(juce::Component *source);
};
