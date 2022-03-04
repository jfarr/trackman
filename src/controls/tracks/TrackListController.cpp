#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(TrackList &trackList) : trackList(trackList), trackListPanel(trackList) {}

void TrackListController::update() {
    trackListPanel.removeAllChildren();
    lanes.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(track);
        lane->addListener(this);
        lanes.push_back(std::unique_ptr<TrackLaneController>(lane));
        trackListPanel.addAndMakeVisible(lane->getTrackLaneControl());
    });
    if (!lanes.empty()) {
        trackListPanel.setSize(
            trackListPanel.getWidth(), lanes.size() * lanes.back()->getTrackLaneControl().getHeight());
    }
}

void TrackListController::selectionChanged(Track &track, juce::Component *source) {
    notifySelectionChanged(track, source);
}

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackListController::notifySelectionChanged(Track &track, juce::Component *source) {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track, source);
    }
}
