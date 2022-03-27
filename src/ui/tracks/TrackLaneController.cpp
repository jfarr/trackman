#include "TrackLaneController.h"
#include "TrackListController.h"
#include "common/listutil.h"

namespace trackman {

TrackLaneController::TrackLaneController(Project &project, Track &track, TrackListController &trackListController,
    AudioTransportSource &transport, AudioFormatManager &formatManager)
    : project(project), track(track), transport(transport), trackListController(trackListController),
      trackLaneControl(project, track, transport), formatManager(formatManager) {
    addListener(&trackListController);
    trackLaneControl.addMouseListener(this, false);
}

TrackLaneController::~TrackLaneController() { removeListener(&trackListController); }

void TrackLaneController::update() {
    if (track.hasMidi()) {
        updateMidi();
    } else if (track.hasSamples()) {
        updateSamples();
    }
    trackLaneControl.update();
}

void TrackLaneController::updateSamples() {
    for (unique_ptr<SampleThumbnail> &thumbnail : sampleThumbnails) {
        thumbnail->removeListener(&trackListController);
    }
    sampleThumbnails.clear();
    trackLaneControl.clear();
    track.eachSample([this](Sample &sample) {
        sampleThumbnails.push_back(make_unique<SampleThumbnail>(project, track, sample, formatManager));
        sampleThumbnails.back()->addListener(&trackListController);
        trackLaneControl.addSampleThumbnail(sampleThumbnails.back().get());
    });
}

void TrackLaneController::updateMidi() {
    for (unique_ptr<NoteCanvas> &canvas : noteCanvases) {
        canvas->onSelected = nullptr;
    }
    noteCanvases.clear();
    trackLaneControl.clear();
    track.eachNoteRoll([this](NoteRoll &noteRoll) {
        noteCanvases.push_back(make_unique<NoteCanvas>(project, track, noteRoll));
        noteCanvases.back()->onSelected = [this](Track &track, NoteRoll &noteRoll) {
            trackListController.noteRollSelected(track, noteRoll);
        };
        noteCanvases.back()->onMouseDragged = [this](NoteCanvas &canvas, int x, int screenY) {
            trackListController.noteRollDragged(canvas, x, screenY);
        };
        trackLaneControl.addNoteCanvas(noteCanvases.back().get());
    });
}

void TrackLaneController::repaint() { trackLaneControl.repaint(); }

void TrackLaneController::mouseDown(const MouseEvent &event) {
    notifySelectionChanged();
    repaint();
}

void TrackLaneController::selectionChanged(Track *track) {
    notifySelectionChanged();
    repaint();
}

void TrackLaneController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void TrackLaneController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void TrackLaneController::notifySelectionChanged() {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(&track);
    }
}

} // namespace trackman
