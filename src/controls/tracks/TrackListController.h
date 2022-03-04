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

    juce::Component &getViewport() { return trackListViewport; }

    void update();
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track) override;

  private:
    TrackList &trackList;
    TrackListPanel trackListPanel;
    juce::Viewport trackListViewport;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
    std::list<TrackListListener *> listeners;

    void notifySelectionChanged(Track &track);
};
