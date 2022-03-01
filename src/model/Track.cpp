#include "Track.h"

Track::Track(juce::String name, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), source(nullptr), listener(nullptr), trackControl(name) {
    trackControl.addListener(this);
    trackControl.setListener(this);
}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
        delete source;
    }
    gain.release();
}

void Track::fileChosen(juce::File file) {
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

void Track::levelChanged(float level) { ((GainAudioSource *)gain.get())->setGain(level); }

void Track::muteChanged(bool muted) {}
