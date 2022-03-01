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
