#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "model/TrackList.h"
#include "TrackListPanel.h"

class TrackListController {
  public:
    TrackListController(TrackList& trackList);
    ~TrackListController() {}

    void update();

  private:
    TrackList& trackList;
    TrackListPanel trackListPanel;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
};
