#include "TrackLaneController.h"
#include "TrackListController.h"
#include "common/listutil.h"

TrackLaneController::TrackLaneController(Project &project, Track &track, TrackListController &trackListController,
    juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : project(project), track(track), transport(transport), trackListController(trackListController),
      trackLaneControl(project, track, transport), formatManager(formatManager) {
    addListener((TrackListListener *)&trackListController);
    trackLaneControl.addMouseListener(this, false);
}

TrackLaneController::~TrackLaneController() { removeListener((TrackListListener *)&trackListController); }

void TrackLaneController::update() {
    for (std::unique_ptr<SampleThumbnail> &thumbnail : thumbnails) {
        thumbnail->removeListener(&trackListController);
    }
    thumbnails.clear();
    trackLaneControl.clear();
    track.eachSample([this](Sample &sample) {
        thumbnails.push_back(std::make_unique<SampleThumbnail>(project, track, sample, formatManager));
        thumbnails.back()->addListener(&trackListController);
        trackLaneControl.addThumbnail(thumbnails.back().get());
    });
    if (track.getMidiMessages().getNumEvents() > 0) {

    }
    trackLaneControl.update();
}

void TrackLaneController::repaint() { trackLaneControl.repaint(); }

void TrackLaneController::mouseDown(const juce::MouseEvent &event) { notifySelectionChanged(); }

void TrackLaneController::selectionChanged(Track *track) { notifySelectionChanged(); }

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
