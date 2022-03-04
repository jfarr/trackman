#include "TrackLaneController.h"
#include "common/listutil.h"

TrackLaneController::TrackLaneController(Track &track) : track(track), trackLaneControl(track) {
    trackLaneControl.addMouseListener(this, true);
}

void TrackLaneController::repaint() {
    trackLaneControl.repaint();
}

void TrackLaneController::mouseDown(const juce::MouseEvent &event) {
    notifySelectionChanged(event.eventComponent);
}

void TrackLaneController::selectionChanged(Track &track, juce::Component *source) {
    if (source != &trackLaneControl) {
        notifySelectionChanged(source);
    }
}

void TrackLaneController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackLaneController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackLaneController::notifySelectionChanged(juce::Component *source) {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track, source);
    }
}
