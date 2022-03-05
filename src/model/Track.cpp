#include "Track.h"

Track::Track(juce::String name) : name(name) {}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate) {
    if (source != nullptr) {
        source->releaseResources();
    }
    source = newSource;
    sampleRate = newSampleRate;
    gain = std::shared_ptr<GainAudioSource>(new GainAudioSource(newSource.get(), false));
}

void Track::loadFile(juce::AudioFormatManager &formatManager, juce::File newFile) {
    if (newFile == juce::File{}) {
        return;
    }
    auto *reader = formatManager.createReaderFor(newFile);
    if (reader != nullptr) {
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        setSource(std::shared_ptr<juce::PositionableAudioSource>(newSource), reader->sampleRate);
        setFile(file);
    }
}

void Track::setLevelGain(float newLevel) {
    level = newLevel;
    if (gain != nullptr) {
        gain->setGain(level);
    }
}

void Track::toggleMute() {
    muted = !muted;
    if (gain != nullptr) {
        gain->setGain(muted ? 0 : level);
    }
}

void Track::setMute(bool newMuted) {
    muted = newMuted;
    if (gain != nullptr) {
        gain->setGain(muted ? 0 : level);
    }
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
}

Sample *Track::addSample(juce::File file, double startPos, double endPos, double length, double sampleRate) {
    samples.push_back(std::make_unique<Sample>(file, startPos, endPos, length, sampleRate));
    return &(*samples.back());
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    std::for_each(samples.begin(), samples.end(), [&f](std::unique_ptr<Sample> &sample) { f(*sample); });
}
