#include "Track.h"

Track::Track(juce::String name) : name(name) {}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

double Track::getSampleRate() const { return source == mixer ? 0.0 : sampleRate; }

double Track::getTotalLengthSeconds() const {
    return (source == nullptr || sampleRate == 0) ? 0 : source->getTotalLength() / sampleRate;
}

void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate) {
    if (source == newSource) {
        return;
    }
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

void Track::loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    if (samples.size() == 0) {
        return;
    }
    mixer = std::make_shared<PositionableMixingAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    for (std::unique_ptr<Sample> &sample : samples) {
        sample->loadFile(formatManager);
        mixer->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
    }
    setSource(mixer, deviceManager.getAudioDeviceSetup().sampleRate);
}

Sample *Track::addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
    juce::File file, double startPos, double endPos, double length, double sampleRate) {
    samples.push_back(std::make_unique<Sample>(file, startPos, endPos, length, sampleRate));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager);
    if (mixer == nullptr) {
        mixer = std::make_shared<PositionableMixingAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    }
    mixer->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
    setSource(mixer, deviceManager.getAudioDeviceSetup().sampleRate);
    return &(*samples.back());
}

Sample *Track::getSelected() const {
    for (std::unique_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->isSelected()) {
            return sample.get();
        }
    }
    return nullptr;
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    for (std::unique_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
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

void Track::selectSample(Sample *selected) {
    eachSample([&selected](Sample &sample) { sample.setSelected(&sample == selected); });
}
