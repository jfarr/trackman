#include "Track.h"

Track::Track(juce::String name, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), source(nullptr), listener(nullptr), trackControl(name) {
    trackControl.addListener(this);
    // gain.processor.setGainLinear(1.0);
}

Track::~Track() {}

void Track::fileChosen(juce::File file) {
    if (listener == nullptr)
        return;

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        // source.reset(new ProcessingAudioSource<juce::dsp::Gain<float>>(newSource, &gain, true));
        // source.reset(newSource);
        listener->onSourceSet(newSource, source, true, reader->sampleRate);
        source = newSource;
    }
}

void Track::levelChanged(float level) { /*gain.processor.setGainLinear(level);*/ }

void Track::muteChanged(bool muted) {}
