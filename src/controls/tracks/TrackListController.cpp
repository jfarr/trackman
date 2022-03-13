#include "TrackListController.h"
#include "common/listutil.h"
#include "controls/MainWindow.h"
#include "controls/desktop/DesktopController.h"

TrackListController::TrackListController(DesktopController &desktopController, juce::AudioTransportSource &transport)
    : desktopController(desktopController), project(desktopController.getProject()),
      trackList(desktopController.getProject().getTrackList()), transport(transport),
      trackListPanel(
          project, trackList, transport, desktopController.getMainWindow().getMainAudioComponent().getFormatManager()) {

    trackListPanel.addListener((SampleListener *)this);
    trackListPanel.addListener((TrackListListener *)this);
    trackListPanel.resized();
}

TrackListController::~TrackListController() {
    trackListPanel.removeListener((SampleListener *)this);
    trackListPanel.removeListener((TrackListListener *)this);
}

void TrackListController::update() {
    lanes.clear();
    trackListPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(project, track, *this, transport,
            desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
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
    desktopController.addNewSample(selected, juce::File(files[0]), x);
    trackListPanel.filesDropped(files, x, y);
}

Sample *TrackListController::addSample(Track &track, juce::File file, int pos) {
    auto *reader =
        desktopController.getMainWindow().getMainAudioComponent().getFormatManager().createReaderFor(juce::File(file));
    if (reader != nullptr) {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        auto length = newSource->getTotalLength() / reader->sampleRate;
        auto scale = project.getHorizontalScale();
        auto width = length * scale;
        auto leftPanelWidth = 25;
        double offset = width / 2;
        double startPos = std::max((pos - offset - leftPanelWidth), 0.0);
        double endPos = startPos + width;
        auto sample = track.addSample(desktopController.getMainWindow().getMainAudioComponent().getDeviceManager(),
            desktopController.getMainWindow().getMainAudioComponent().getFormatManager(), file, startPos / scale,
            endPos / scale, length, reader->sampleRate);
        trackList.adjustTrackLengths();
        selectionChanged(&track);
        updateLane(track);
        updateMixerSource();
        return sample;
    }
    return nullptr;
}

void TrackListController::moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos) {
    if (&fromTrack != &toTrack) {
        fromTrack.moveSampleTo(sample, toTrack);
        selectionChanged(&toTrack);
        fromTrack.selectSample(nullptr);
        toTrack.selectSample(&sample);
    }
    sample.setPosition(pos);
    trackList.adjustTrackLengths();
    trackListPanel.resize();
}

void TrackListController::resizeSample(Sample &sample, double length) {
    sample.setLength(length);
    trackList.adjustTrackLengths();
    trackListPanel.resize();
}

void TrackListController::deleteSample(Track &track, Sample *sample) {
    auto pos = transport.getCurrentPosition();
    if (sample == nullptr) {
        return;
    }
    track.deleteSample(sample);
    updateLane(track);
    updateMixerSource();
    trackList.adjustTrackLengths();
    pos = std::max(pos, transport.getLengthInSeconds());
    transport.setPosition(pos);
}

void TrackListController::undeleteSample(Track &track, Sample *sample) {
    auto pos = transport.getCurrentPosition();
    if (sample == nullptr) {
        return;
    }
    track.undeleteSample(sample);
    updateLane(track);
    updateMixerSource();
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
        desktopController.selectionChanged(track);
    }
}

void TrackListController::sampleSelected(Track &track, Sample &sample) {
    selectingSample = true;
    trackList.selectSample(&sample);
    selectionChanged(&track);
}

void TrackListController::sampleMoved(Track &track, Sample &sample, int x, int y) {
    removeDragLane();
    auto curPos = sample.getStartPos();
    auto length = sample.getLengthSecs();
    auto scale = project.getHorizontalScale();
    auto width = length * scale;
    auto leftPanelWidth = 25;
    double offset = width / 2;
    double newPos = std::max((x - offset - leftPanelWidth), 0.0) / scale;
    Track *toTrack = trackListPanel.getTrackAtPos(x, y);
    desktopController.moveSelectedSample(sample, track, toTrack, curPos, newPos);
}

void TrackListController::sampleResized(Sample &sample, int width) {
    auto curLen = sample.getLengthSecs();
    auto newLen = std::max(width, 2) / project.getHorizontalScale();
    desktopController.resizeSample(sample, curLen, newLen);
}

void TrackListController::mouseDragged(SampleThumbnail &thumbnail, int x, int screenY) {
    thumbnail.setTopLeftPosition(thumbnail.getPosition().withX(x));
    auto y = screenY - trackListPanel.getScreenPosition().getY();
    auto track = trackListPanel.getTrackAtPos(x, y);
    if (track != currentDragTrack) {
        currentDragTrack = track;
        TrackLaneController *lane;
        if (track == nullptr) {
            if (newDragLane == nullptr) {
                track = new Track(trackList);
                newDragLane =
                    new TrackLaneController(project, *track, *this, transport, desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
                trackListPanel.addLane(&newDragLane->getTrackLaneControl());
                trackListPanel.addAndMakeVisible(&newDragLane->getTrackLaneControl());
                trackListPanel.resize();
            }
            lane = newDragLane;
        } else {
            lane = getLane(*track);
        }
        if (lane != nullptr) {
            getLane(thumbnail.getTrack())->getTrackLaneControl().removeChildComponent(&thumbnail);
            lane->getTrackLaneControl().addAndMakeVisible(thumbnail);
        }
    }
}

void TrackListController::dragEnded() {
    removeDragLane();
    updateLanes();
}

void TrackListController::removeDragLane() {
    if (newDragLane != nullptr) {
        trackListPanel.removeChildComponent(&newDragLane->getTrackLaneControl());
        trackListPanel.removeLane(&newDragLane->getTrackLaneControl());
        trackListPanel.resize();
        delete &newDragLane->getTrack();
        delete newDragLane;
        newDragLane = nullptr;
    }
    currentDragTrack = nullptr;
}

void TrackListController::updateMixerSource() {
    //    desktopController.getMixerController().updateAudioSource();
}
