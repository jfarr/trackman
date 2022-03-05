#include "TrackLaneController.h"
#include "common/listutil.h"

TrackLaneController::TrackLaneController(Track &track) : track(track), trackLaneControl(track) {
    trackLaneControl.addMouseListener(this, true);
}

void TrackLaneController::repaint() { trackLaneControl.repaint(); }

void TrackLaneController::mouseDown(const juce::MouseEvent &event) { notifySelectionChanged(); }

void TrackLaneController::selectionChanged(Track &track) { notifySelectionChanged(); }

void TrackLaneController::addListener(TrackListListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void TrackLaneController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackLaneController::notifySelectionChanged() {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track);
    }
}
