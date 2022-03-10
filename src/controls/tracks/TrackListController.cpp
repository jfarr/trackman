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
    trackListPanel.addListener((SampleListener *) this);
    trackListPanel.addListener((TrackListListener *) this);
    trackListPanel.resized();
}

TrackListController::~TrackListController() {
    trackListPanel.removeListener((SampleListener *) this);
    trackListPanel.removeListener((TrackListListener *) this);
}

void TrackListController::update() {
    for (std::unique_ptr<TrackLaneController> &lane : lanes) {
        lane->removeListener((TrackListListener *)this);
        lane->removeListener((SampleListener *)this);
    }
    lanes.clear();
    trackListPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(track, transport, formatManager);
        lane->addListener((TrackListListener *)this);
        lane->addListener((SampleListener *)this);
        lanes.push_back(std::unique_ptr<TrackLaneController>(lane));
        trackListPanel.addLane(&lane->getTrackLaneControl());
        lane->update();
    });
    trackListPanel.update();
    trackList.adjustTrackLengths();
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
    notifySampleAdded(selected, juce::File(files[0]), x);
    trackListPanel.filesDropped(files, x, y);
}

Sample *TrackListController::addSample(Track &track, juce::File file, int pos) {
    auto *reader = formatManager.createReaderFor(juce::File(file));
    if (reader != nullptr) {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        auto length = newSource->getTotalLength() / reader->sampleRate;
        auto width = length * scale;
        auto leftPanelWidth = 25;
        double offset = width / 2;
        double startPos = std::max((pos - offset - leftPanelWidth), 0.0);
        double endPos = startPos + width;
        auto sample = track.addSample(
            deviceManager, formatManager, file, startPos / scale, endPos / scale, length, reader->sampleRate);
        trackList.adjustTrackLengths();
        selectionChanged(&track);
        updateLane(track);
        if (listener != nullptr) {
            listener->onSourceSet();
        }
        return sample;
    }
    return nullptr;
}

void TrackListController::deleteSample(Track &track, Sample *sample) {
    auto pos =  transport.getCurrentPosition();
    if (sample == nullptr) {
        return;
    }
    track.deleteSample(sample);
    updateLane(track);
    if (listener != nullptr) {
        listener->onSourceSet();
    }
    trackList.adjustTrackLengths();
    pos = std::max(pos, transport.getLengthInSeconds());
    transport.setPosition(pos);
    // delete new track
}

void TrackListController::undeleteSample(Track &track, Sample *sample) {
    auto pos =  transport.getCurrentPosition();
    if (sample == nullptr) {
        return;
    }
    track.undeleteSample(sample);
    updateLane(track);
    if (listener != nullptr) {
        listener->onSourceSet();
    }
    transport.setPosition(pos);
    trackList.adjustTrackLengths();
}

void TrackListController::updateLane(Track &track) {
    TrackLaneController *lane = getLane(track);
    if (lane != nullptr) {
        lane->update();
    }
}

TrackLaneController *TrackListController::getLane(Track &track) {
    for (auto &lane : lanes) {
        if (&lane->getTrack() == &track) {
            return lane.get();
        }
    }
    return nullptr;
}

void TrackListController::updateLanes() {
    for (auto &lane : lanes) {
        lane->update();
    }
}

void TrackListController::selectionChanged(Track *track) {
    if (!selectingSample) {
        trackList.selectSample(nullptr);
    }
    selectingSample = false;
    if (track != selected) {
        selected = track;
        notifySelectionChanged(track);
        updateLanes();
    }
}

void TrackListController::sampleSelected(Track &track, Sample &sample) {
    selectingSample = true;
    trackList.selectSample(&sample);
}

void TrackListController::sampleMoved(Sample &sample, int x) {
    auto length = sample.getLengthSecs();
    auto width = length * scale;
    auto leftPanelWidth = 25;
    double offset = width / 2;
    double startPos = std::max((x - offset - leftPanelWidth), 0.0);
    sample.setPosition(startPos / scale);
    trackList.adjustTrackLengths();
    trackListPanel.resize();
}

void TrackListController::sampleResized(Sample &sample, int width) {
    auto newLength = width / scale;
    sample.setLength(newLength);
    trackList.adjustTrackLengths();
    trackListPanel.resize();
}

void TrackListController::dragEnded() { updateLanes(); }

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void TrackListController::notifySelectionChanged(Track *track) {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(track);
    }
}

void TrackListController::addListener(SampleListener *listener) {
    if (!listContains(sampleListeners, listener)) {
        sampleListeners.push_front(listener);
    }
}

void TrackListController::removeListener(SampleListener *listener) { sampleListeners.remove(listener); }

void TrackListController::notifySampleAdded(Track *track, juce::File file, int pos) {
    for (SampleListener *listener : sampleListeners) {
        listener->sampleAdded(track, file, pos);
    }
}
