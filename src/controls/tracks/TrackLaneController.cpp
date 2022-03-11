#include "TrackLaneController.h"
#include "TrackListController.h"
#include "common/listutil.h"

TrackLaneController::TrackLaneController(TrackListController &trackListController, Track &track,
    juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : trackListController(trackListController), track(track), transport(transport), trackLaneControl(track, transport),
      formatManager(formatManager) {
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
        thumbnails.push_back(std::make_unique<SampleThumbnail>(track, sample, transport, formatManager));
        thumbnails.back()->addListener(&trackListController);
        trackLaneControl.addThumbnail(thumbnails.back().get());
    });
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
