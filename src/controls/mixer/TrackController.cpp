#include "TrackController.h"
#include "MixerController.h"
#include "common/listutil.h"

TrackController::TrackController(
    MixerController &mixerController, Track &track, TrackControl &trackControl, juce::AudioFormatManager &formatManager)
    : mixerController(mixerController), formatManager(formatManager), track(track), trackControl(trackControl) {
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

void TrackController::toggleSolo(Track &track) {
    track.toggleSolo();
    trackControl.update();
}

void TrackController::repaint() { trackControl.repaint(); }

void TrackController::nameChanged(Track &track, juce::String newName) { mixerController.nameChanged(track, newName); }

void TrackController::levelChanged(Track &track, float newLevel) { track.setLevelGain(newLevel); }

void TrackController::levelChangeFinalized(Track &track, float previousLevel) {
    mixerController.levelChangeFinalized(track, previousLevel);
}

void TrackController::muteToggled(Track &track) {
    track.toggleMute();
    mixerController.muteToggled(track);
}

void TrackController::soloToggled(Track &track) {
    track.toggleSolo();
    mixerController.soloToggled(track);
}

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
