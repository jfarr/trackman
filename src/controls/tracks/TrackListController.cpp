#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(TrackList &trackList)
    : trackList(trackList), trackListPanel(trackList, trackListViewport) {
    trackListViewport.setSize(800, 350);
    trackListViewport.setScrollBarsShown(true, true);
    trackListViewport.setViewedComponent(&trackListPanel, false);
    trackListPanel.resized();
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

void TrackListController::fileDragEnter(const juce::StringArray &files, int x, int y) {
    trackListPanel.fileDragEnter(files, x, y);
}

void TrackListController::fileDragMove(const juce::StringArray &files, int x, int y) {
    trackListPanel.fileDragMove(files, x, y);
}

void TrackListController::fileDragExit(const juce::StringArray &files) { trackListPanel.fileDragExit(files); }

void TrackListController::filesDropped(const juce::StringArray &files, int x, int y) {
    Track *selected = trackListPanel.getTrackAtPos(x, y);
    if (selected != nullptr) {
        double offset = trackListPanel.getDropBox().getWidth() / 2;
        double startPos = (x - offset) / scale;
        double endPos = (x + offset) / scale;
        selected->addSample(files[0], startPos, endPos, 0, 0);
        selectionChanged(*selected);
    }
    trackListPanel.filesDropped(files, x, y);
}

void TrackListController::selectionChanged(Track &track) { notifySelectionChanged(track); }

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackListController::notifySelectionChanged(Track &track) {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track);
    }
}
