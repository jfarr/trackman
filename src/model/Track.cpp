#include "Track.h"

Track::Track(juce::String name, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), source(nullptr), listener(nullptr), trackControl(name) {
    trackControl.addListener(this);
    // gain.processor.setGainLinear(1.0);
}

Track::~Track() {
//    gain->releaseResources();
//    gain.release();
}

void Track::fileChosen(juce::File file) {
    if (listener == nullptr)
        return;

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        // source.reset(new ProcessingAudioSource<juce::dsp::Gain<float>>(newSource, &gain, true));
        // source.reset(newSource);
        auto newGain = new GainAudioSource(newSource, true);
        gain.reset(newGain);
        listener->onSourceSet(newGain, source, true, reader->sampleRate);
        source = newGain;
        gain.release();
    }
}

void Track::levelChanged(float level) { ((GainAudioSource *)gain.get())->setGain(level); }

void Track::muteChanged(bool muted) {}
