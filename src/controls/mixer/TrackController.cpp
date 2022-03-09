#include "TrackController.h"
#include "common/listutil.h"

TrackController::TrackController(Track &track, TrackControl &trackControl, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), track(track), trackControl(trackControl) {
    trackControl.addListener(this);
    trackControl.addMouseListener(this, true);
}

TrackController::~TrackController() {
    trackControl.removeListener(this);
    trackControl.removeMouseListener(this);
}

void TrackController::setLevel(float newLevel) {
    track.setLevelGain(newLevel);
    trackControl.update();
}

void TrackController::toggleMute(Track &track) {
    track.toggleMute();
    trackControl.update();
}

void TrackController::repaint() { trackControl.repaint(); }

void TrackController::nameChanged(Track &track, juce::String newName) { notifyNameChanged(track, newName); }

void TrackController::levelChanged(Track &track, float newLevel) { track.setLevelGain(newLevel); }

void TrackController::muteToggled(Track &track) { track.toggleMute(); }

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

void TrackController::addListener(TrackControlListener *listener) {
    if (!listContains(trackControlListeners, listener)) {
        trackControlListeners.push_front(listener);
    }
}

void TrackController::removeListener(TrackControlListener *listener) {
    trackControlListeners.remove(listener);
}

void TrackController::notifyNameChanged(Track &track, juce::String newName) {
    for (TrackControlListener *listener : trackControlListeners) {
        listener->nameChanged(track, newName);
    }
}
