#include "Track.h"

Track::Track(juce::String name, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), source(nullptr), listener(nullptr), trackControl(name) {
    trackControl.addListener(this);
    trackControl.setListener(this);
}

Track::~Track() { gain.release(); }

void Track::fileChosen(juce::File file) {
    if (listener == nullptr) {
        return;
    }

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        auto newGain = new GainAudioSource(newSource, true);
        gain.reset(newGain);
        listener->onSourceSet(newGain, source, true, reader->sampleRate);
        source = newGain;
    }
}

void Track::levelChanged(float level) { ((GainAudioSource *)gain.get())->setGain(level); }

void Track::muteChanged(bool muted) {}
