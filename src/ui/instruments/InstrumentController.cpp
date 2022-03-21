#include "InstrumentController.h"
#include "InstrumentsController.h"
#include "common/listutil.h"

namespace trackman {

InstrumentController::InstrumentController(InstrumentsController &instrumentsController, Track &track)
    : track(track), instrumentControl(track) {
    addListener((TrackListListener *)&instrumentsController);
    instrumentControl.addMouseListener(this, false);
}

void InstrumentController::update() { instrumentControl.update(); }

void InstrumentController::repaint() { instrumentControl.repaint(); }

void InstrumentController::mouseDown(const MouseEvent &event) { notifySelectionChanged(); }

void InstrumentController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void InstrumentController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void InstrumentController::notifySelectionChanged() {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(&track);
    }
}

} // namespace trackman
