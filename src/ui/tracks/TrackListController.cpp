#include "TrackListController.h"
#include "common/listutil.h"
#include "ui/MainWindow.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TrackListController::TrackListController(DesktopController &desktopController)
    : desktopController(desktopController), project(desktopController.getProject()),
      trackListViewport(desktopController.getDesktopComponent(), desktopController.getProject()),
      trackListPanel(
          desktopController, trackListViewport, desktopController.getProject().getTransport().getTransportSource()) {

    trackListPanel.onMoveNoteRoll = [this](Track &track, NoteRoll &noteRoll, int x, int y) {
        noteRollMoved(track, noteRoll, x, y);
    };
    trackListPanel.onDragEnded = [this]() { dragEnded(); };
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
    project.getTrackList().eachTrack([this](Track &track) {
        auto lane = new TrackLaneController(project, track, *this, project.getTransport().getTransportSource(),
            desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
        lanes.push_back(unique_ptr<TrackLaneController>(lane));
        trackListPanel.addLane(&lane->getTrackLaneControl());
        lane->update();
    });
    trackListPanel.update();
}

void TrackListController::repaint() {
    for (unique_ptr<TrackLaneController> &lane : lanes) {
        lane->repaint();
    }
}

void TrackListController::fileDragEnter(const StringArray &files, int x, int y) {
    trackListPanel.fileDragEnter(files, x, y);
}

void TrackListController::fileDragMove(const StringArray &files, int x, int y) {
    trackListPanel.fileDragMove(files, x, y);
}

void TrackListController::fileDragExit(const StringArray &files) { trackListPanel.fileDragExit(files); }

void TrackListController::filesDropped(const StringArray &files, int x, int y) {
    Track *selected = trackListPanel.getTrackAtPos(x, y);
    desktopController.addNewSample(selected, File(files[0]), x);
    trackListPanel.filesDropped(files, x, y);
}

Sample *TrackListController::addSample(Track &track, File file, int pos) {
    auto *reader =
        desktopController.getMainWindow().getMainAudioComponent().getFormatManager().createReaderFor(File(file));
    if (reader != nullptr) {
        auto newSource = make_unique<AudioFormatReaderSource>(reader, true);
        auto length = newSource->getTotalLength() / reader->sampleRate;
        auto scale = project.getHorizontalScale();
        auto width = length * scale;
        double offset = width / 2;
        double startPos = max((pos - offset), 0.0);
        double endPos = startPos + width;
        auto sample = project.addSample(track, file, startPos / scale, endPos / scale,
            desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
        selectionChanged(&track);
        updateLane(track);
        return sample;
    }
    return nullptr;
}

void TrackListController::moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos) {
    if (&fromTrack != &toTrack) {
        project.getMixer().removeSource(toTrack.getSource());
        if (fromTrack.getNumSamples() == 1) {
            project.getMixer().removeSource(fromTrack.getSource());
        }
        fromTrack.moveSampleToTrack(sample, toTrack);
        selectionChanged(&toTrack);
        fromTrack.selectSample(nullptr);
        toTrack.selectSample(&sample);
        project.getMixer().addSource(toTrack.getSource());
    }
    sample.setPosition(pos);
    trackListPanel.resize();
}

void TrackListController::resizeSample(Sample &sample, double length) {
    sample.setLength(length);
    trackListPanel.resize();
}

void TrackListController::deleteSample(Track &track, Sample *sample) {
    if (sample != nullptr) {
        sample->setDeleted(true);
        updateLane(track);
    }
}

void TrackListController::undeleteSample(Track &track, Sample *sample) {
    if (sample != nullptr) {
        sample->setDeleted(false);
        updateLane(track);
    }
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
        project.getTrackList().selectSample(nullptr);
    }
    if (!selectingNoteRoll) {
        project.getTrackList().selectNoteRoll(nullptr);
    }
    selectingSample = false;
    selectingNoteRoll = false;
    selected = track;
    desktopController.selectionChanged(track);
}

void TrackListController::noteRollSelected(Track &track, NoteRoll &noteRoll) {
    selectingNoteRoll = true;
    project.getTrackList().selectNoteRoll(&noteRoll);
    selectionChanged(&track);
    trackListPanel.repaint();
}

void TrackListController::deleteNoteRoll(Track &track, NoteRoll *noteRoll) {
    if (noteRoll != nullptr) {
        noteRoll->setDeleted(true);
        updateLane(track);
    }
}

void TrackListController::undeleteNoteRoll(Track &track, NoteRoll *noteRoll) {
    if (noteRoll != nullptr) {
        noteRoll->setDeleted(false);
        updateLane(track);
    }
}

void TrackListController::noteRollDragged(NoteCanvas &canvas, int x, int screenY) {
    x = max(x, 0);
    auto scale = project.getHorizontalScale();
    auto positionInSeconds = x / scale;
    auto position = project.positionAtTime(positionInSeconds);
    auto newX = project.timeAtPosition(position.rounded()) * scale;
    canvas.setTopLeftPosition(canvas.getPosition().withX(newX));

    auto y = screenY - trackListPanel.getScreenPosition().getY();
    auto track = trackListPanel.getTrackAtPos(x, y);
    if (track != currentDragTrack) {
        currentDragTrack = track;
        TrackLaneController *lane;
        if (track == nullptr) {
            if (newDragLane == nullptr) {
                track = new Track(project, desktopController.getDeviceManager());
                newDragLane =
                    new TrackLaneController(project, *track, *this, project.getTransport().getTransportSource(),
                        desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
                trackListPanel.addLane(&newDragLane->getTrackLaneControl());
                trackListPanel.addAndMakeVisible(&newDragLane->getTrackLaneControl());
                trackListPanel.resize();
            }
            lane = newDragLane;
        } else {
            lane = getLane(*track);
        }
        if (lane != nullptr) {
            getLane(canvas.getTrack())->getTrackLaneControl().removeChildComponent(&canvas);
            lane->getTrackLaneControl().addAndMakeVisible(canvas);
        }
    }
}

void TrackListController::noteRollMoved(Track &track, NoteRoll &noteRoll, int x, int y) {
    removeDragLane();
    auto curPos = noteRoll.getStartPosInSeconds();
    auto scale = project.getHorizontalScale();
    x = max(x, 0);
    auto positionInSeconds = x / scale;
    auto position = project.positionAtTime(positionInSeconds);
    auto newPos = project.timeAtPosition(position.rounded());
    if (newPos != curPos) {
        Track *toTrack = trackListPanel.getTrackAtPos(x, y);
        desktopController.moveSelectedNoteRoll(noteRoll, track, toTrack, curPos, newPos);
    }
}

void TrackListController::moveNoteRoll(NoteRoll &noteRoll, Track &fromTrack, Track &toTrack, double pos) {
    if (&fromTrack != &toTrack) {
        project.getMixer().removeSource(toTrack.getSource());
        if (fromTrack.getNumSamples() == 1) {
            project.getMixer().removeSource(fromTrack.getSource());
        }
        fromTrack.moveNoteRollToTrack(noteRoll, toTrack);
        selectionChanged(&toTrack);
        fromTrack.selectSample(nullptr);
        toTrack.selectNoteRoll(&noteRoll);
        project.getMixer().addSource(toTrack.getSource());
    }
    noteRoll.setPosition(pos);
    trackListPanel.resize();
}

void TrackListController::sampleSelected(Track &track, Sample &sample) {
    selectingSample = true;
    project.getTrackList().selectSample(&sample);
    selectionChanged(&track);
}

void TrackListController::sampleMoved(Track &track, Sample &sample, int x, int y) {
    removeDragLane();
    auto curPos = sample.getStartPosInSeconds();
    auto scale = project.getHorizontalScale();
    x = max(x, 0);
    double newPos = (double)x / scale;
    if (newPos != curPos) {
        Track *toTrack = trackListPanel.getTrackAtPos(x, y);
        desktopController.moveSelectedSample(sample, track, toTrack, curPos, newPos);
    }
}

void TrackListController::sampleResized(Sample &sample, int width) {
    auto curLen = sample.getLengthInSeconds();
    auto newLen = max(width, 2) / project.getHorizontalScale();
    if (newLen != curLen) {
        desktopController.resizeSample(sample, curLen, newLen);
    }
}

void TrackListController::sampleDragged(SampleThumbnail &thumbnail, int x, int screenY) {
    x = max(x, 0);
    thumbnail.setTopLeftPosition(thumbnail.getPosition().withX(x));
    auto y = screenY - trackListPanel.getScreenPosition().getY();
    auto track = trackListPanel.getTrackAtPos(x, y);
    if (track != currentDragTrack) {
        currentDragTrack = track;
        TrackLaneController *lane;
        if (track == nullptr) {
            if (newDragLane == nullptr) {
                track = new Track(project, desktopController.getDeviceManager());
                newDragLane =
                    new TrackLaneController(project, *track, *this, project.getTransport().getTransportSource(),
                        desktopController.getMainWindow().getMainAudioComponent().getFormatManager());
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

} // namespace trackman
