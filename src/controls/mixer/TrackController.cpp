#include "TrackController.h"
#include "common/listutil.h"
#include "controls/desktop/DesktopController.h"

TrackController::TrackController(
    DesktopController &desktopController, Track &track, juce::AudioFormatManager &formatManager)
    : desktopController(desktopController), track(track), formatManager(formatManager), trackControl(track) {
    trackControl.addListener(&desktopController);
    trackControl.addListener(this);
    trackControl.addMouseListener(this, true);
}

TrackController::~TrackController() {
    trackControl.removeListener(&desktopController);
    trackControl.removeListener(this);
    trackControl.removeMouseListener(this);
}

void TrackController::setLevel(float newLevel) {
    track.setLevelGain(newLevel);
    trackControl.update();
}

void TrackController::setMute(Track &track, bool newMute) {
    track.setMute(newMute);
    trackControl.update();
}

void TrackController::setSolo(Track &track, bool newSolo) {
    track.setSolo(newSolo);
    trackControl.update();
}

void TrackController::repaint() { trackControl.repaint(); }

void TrackController::trackLevelChanged(Track &track, float newLevel) { track.setLevelGain(newLevel); }

void TrackController::mouseDown(const juce::MouseEvent &event) { notifySelectionChanged(); }

void TrackController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void TrackController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void TrackController::notifySelectionChanged() {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(&track);
    }
}
