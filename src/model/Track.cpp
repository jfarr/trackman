#include "Track.h"
#include "TrackList.h"
#include "audio/MixerAudioSource.h"

Track::Track(TrackList &trackList) : trackList(trackList) {}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

double Track::getSampleRate() const { return source == mixer ? 0.0 : sampleRate; }

double Track::getTotalLengthSeconds() const {
    double len = 0;
    for (std::unique_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->getSource() != nullptr) {
            len = std::max(len, sample->getEndPos());
        }
    }
    return len;
}

void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate) {
    DBG("Track::setSource - set track source: " << getName());
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

void Track::loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    if (samples.size() == 0) {
        return;
    }
    mixer = std::make_shared<MixerAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    for (std::unique_ptr<Sample> &sample : samples) {
        sample->loadFile(formatManager);
        if (sample->getSource() != nullptr) {
            mixer->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
        }
    }
    setSource(mixer, deviceManager.getAudioDeviceSetup().sampleRate);
}

Sample *Track::addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
    juce::File file, double startPos, double endPos, double length, double sampleRate) {
    samples.push_back(std::make_unique<Sample>(file, startPos, endPos, length, sampleRate));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager);
    if (mixer == nullptr) {
        mixer = std::make_shared<MixerAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    }
    if (sample->getSource() != nullptr) {
        mixer->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
    }
    setSource(mixer, deviceManager.getAudioDeviceSetup().sampleRate);
    if (name == "") {
        name = file.getFileName();
    }
    return &(*samples.back());
}

void Track::adjustSampleLengthSecs(double newLen) {
    eachSample([newLen](Sample &sample) { sample.setMinLengthSecs(newLen); });
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

void Track::toggleMute() { setMute(!muted); }

void Track::toggleSolo() { setSolo(!soloed); }

void Track::setMute(bool newMuted) {
    DBG("Track " << number << " mute: " << (newMuted ? "true" : "false"));
    muted = newMuted;
    if (gain != nullptr) {
        gain->setGain(muted ? 0 : level);
    }
}

void Track::setSolo(bool newSoloed) {
    DBG("Track " << number << " solo: " << (newSoloed ? "true" : "false"));
    soloed = newSoloed;
    trackList.soloTracks();
}

void Track::setGain() {
    if (gain != nullptr) {
        bool solo = trackList.getSoloed().size() == 0 || soloed;
        DBG("Track " << number << " set gain: " << (solo ? level : 0));
        gain->setGain(solo ? level : 0);
    }
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
}

void Track::selectSample(Sample *selected) {
    eachSample([&selected](Sample &sample) { sample.setSelected(&sample == selected); });
}

void Track::deleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(true);
    mixer->removeInputSource(sample->getSource());
}

void Track::undeleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(false);
    mixer->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
}
