#include "TrackListController.h"

TrackListController::TrackListController(TrackList& trackList) : trackList(trackList), trackListPanel(trackList) {

}

void TrackListController::update() {
    trackListPanel.removeAllChildren();
    lanes.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(track);
        lanes.push_back(std::unique_ptr<TrackLaneController>(lane));
        trackListPanel.addAndMakeVisible(lane->getTrackLaneControl());
    });
    if (!lanes.empty()) {
        trackListPanel.setSize(trackListPanel.getWidth(), lanes.size() * lanes.back()->getTrackLaneControl().getHeight());
    }
}
