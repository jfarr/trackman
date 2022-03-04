#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(TrackList &trackList) : trackList(trackList), trackListPanel(trackList) {
    trackListViewport.setSize(800, 350);
    trackListViewport.setScrollBarsShown(true, true);
    trackListViewport.setViewedComponent(&trackListPanel, false);
}

void TrackListController::update() {
    lanes.clear();
    trackListPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(track);
        lane->addListener(this);
        lanes.push_back(std::unique_ptr<TrackLaneController>(lane));
        trackListPanel.addLane(&lane->getTrackLaneControl());
    });
    trackListPanel.update();
}

void TrackListController::repaint() {
    for (std::unique_ptr<TrackLaneController> &lane : lanes) {
        lane->repaint();
    }
}

void TrackListController::selectionChanged(Track &track) {
    notifySelectionChanged(track);
}

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackListController::notifySelectionChanged(Track &track) {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track);
    }
}
