#include "TrackController.h"

TrackController::TrackController(Track &track, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), track(track), source(nullptr), listener(nullptr), trackControl(track.getName()) {
    trackControl.addListener(this);
    trackControl.setListener(this);
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

void TrackController::levelChanged(float level) { ((GainAudioSource *)gain.get())->setGain(level); }

void TrackController::muteChanged(bool muted) {}
