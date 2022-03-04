#include "TrackController.h"
#include "common/listutil.h"

TrackController::TrackController(Track &track, TrackControl &trackControl, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), track(track), trackControl(trackControl) {
    trackControl.addListener(this);
    trackControl.setListener(this);
    trackControl.addMouseListener(this, true);
}

TrackController::~TrackController() {}

void TrackController::setLevel(float newLevel) {
    track.setLevel(newLevel);
    trackControl.update();
}

void TrackController::repaint() { trackControl.repaint(); }

void TrackController::fileChosen(juce::File file) {
    if (listener == nullptr) {
        return;
    }
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        track.setSource(std::shared_ptr<juce::PositionableAudioSource>(newSource), reader->sampleRate);
        track.setFile(file);
        listener->onSourceSet();
    }
}

void TrackController::levelChanged(float newLevel) { track.setLevel(newLevel); }

void TrackController::muteToggled() { track.toggleMute(); }

void TrackController::mouseDown(const juce::MouseEvent &event) { notifySelectionChanged(); }

void TrackController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackController::notifySelectionChanged() {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track);
    }
}
