#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(TrackList &trackList, juce::AudioTransportSource &transport,
    juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager)
    : trackList(trackList), transport(transport),
      trackListPanel(trackList, trackListViewport, transport, formatManager), deviceManager(deviceManager),
      formatManager(formatManager) {
    trackListViewport.setSize(800, 350);
    trackListViewport.setScrollBarsShown(true, true);
    trackListViewport.setViewedComponent(&trackListPanel, false);
    trackListPanel.addListener(this);
    trackListPanel.resized();
}

void TrackListController::update() {
    lanes.clear();
    trackListPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(track, transport, formatManager);
        lane->addListener(this);
        lanes.push_back(std::unique_ptr<TrackLaneController>(lane));
        trackListPanel.addLane(&lane->getTrackLaneControl());
        lane->update();
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
        auto *reader = formatManager.createReaderFor(juce::File(files[0]));
        if (reader != nullptr) {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
            auto length = newSource->getTotalLength() / reader->sampleRate;
            auto width = length * scale;
            auto leftPanelWidth = 25;
            double offset = width / 2;
            double startPos = std::max((x - offset - leftPanelWidth), 0.0);
            double endPos = startPos + width;
            selected->addSample(
                deviceManager, formatManager, files[0], startPos / scale, endPos / scale, length, reader->sampleRate);
        }
        selectionChanged(*selected);
        updateLane(*selected);
        listener->onSourceSet();
    }
    trackListPanel.filesDropped(files, x, y);
}

void TrackListController::updateLane(Track &track) {
    TrackLaneController *lane = getLane(track);
    if (lane != nullptr) {
        lane->update();
    }
}

TrackLaneController *TrackListController::getLane(Track &track) {
    for (std::unique_ptr<TrackLaneController> &lane : lanes) {
        if (&lane->getTrack() == &track) {
            return lane.get();
        }
    }
    return nullptr;
}

void TrackListController::selectionChanged(Track &track) { notifySelectionChanged(track); }

void TrackListController::sampleDropped(SampleThumbnail *thumbnail, juce::Point<int> pos) {
    auto length = thumbnail->getSample().getLength();
    auto width = length * scale;
    auto leftPanelWidth = 25;
    double offset = width / 2;
    double startPos = std::max((pos.getX() - offset - leftPanelWidth), 0.0);
    thumbnail->getSample().setPosition(startPos / scale);
    trackListPanel.resize();
}

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
