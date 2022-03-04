#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "TrackListPanel.h"
#include "controls/desktop/TrackListListener.h"
#include "model/TrackList.h"

class TrackListController : public TrackListListener {
  public:
    TrackListController(TrackList &trackList);
    ~TrackListController() {}

    void update();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track, juce::Component *source) override;

  private:
    TrackList &trackList;
    TrackListPanel trackListPanel;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
    std::list<TrackListListener *> listeners;

    void notifySelectionChanged(Track &track, juce::Component *source);
};
