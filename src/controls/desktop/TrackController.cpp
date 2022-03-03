#include "TrackController.h"
#include "common/listutil.h"

TrackController::TrackController(Track &track, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), track(track), source(nullptr), listener(nullptr), trackControl(track.getName()),
      trackLaneControl(track.getName()) {
    trackControl.addListener(this);
    trackControl.setListener(this);
    trackControl.addMouseListener(this, true);
    trackLaneControl.addMouseListener(this, true);
}

TrackController::~TrackController() {
    if (source != nullptr) {
        source->releaseResources();
        delete source;
    }
    gain.release();
}

void TrackController::fileChosen(juce::File file) {
    if (listener == nullptr) {
        return;
    }

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        if (source != nullptr) {
            source->releaseResources();
        }
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        auto newGain = new GainAudioSource(newSource, true);
        gain.reset(newGain);
        listener->onSourceSet(newGain, source, false, reader->sampleRate);
        source = newGain;
    }
}

void TrackController::levelChanged(float newLevel) {
    level = newLevel;
    auto gainSource = (GainAudioSource *)gain.get();
    if (gainSource != nullptr) {
        gainSource->setGain(newLevel);
    }
}

void TrackController::muteToggled() {
    muted = !muted;
    auto newLevel = (muted ? 0.0 : level);
    auto gainSource = (GainAudioSource *)gain.get();
    if (gainSource != nullptr) {
        gainSource->setGain(newLevel);
    }
}

void TrackController::mouseDown(const juce::MouseEvent &event) {
    auto pos = event.getScreenPosition();
    if (trackControl.getScreenBounds().contains(pos) || trackLaneControl.getScreenBounds().contains(pos)) {
        notifySelectionChanged();
    }
}

void TrackController::setSelected(bool newSelected) {
    selected = newSelected;
    trackControl.setSelected(selected);
    trackLaneControl.setSelected(selected);
}

void TrackController::addListener(TrackControllerListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackController::removeListener(TrackControllerListener *listener) { listeners.remove(listener); }

void TrackController::notifySelectionChanged() {
    for (TrackControllerListener *listener : listeners) {
        listener->selectionChanged(this);
    }
}
