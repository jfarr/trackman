#include "Track.h"

Track::Track(juce::String name, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), trackControl(name) {
    trackControl.addListener(this);
    gain.processor.setGainLinear(1.0);
}

Track::~Track() {
}

void Track::fileChosen(juce::File file) {
    if (listener == nullptr)
        return;

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        // source.reset(new ProcessingAudioSource<juce::dsp::Gain<float>>(newSource, &gain, true));
        //source.reset(newSource);
        listener->onSourceSet(newSource, true, reader->sampleRate);
    }
}

void Track::levelChanged(float level) { gain.processor.setGainDecibels(level); }

void Track::muteChanged(bool muted) {}
