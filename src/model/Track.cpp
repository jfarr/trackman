#include "Track.h"
#include "TrackList.h"
#include "audio/PositionableMixingAudioSource.h"

Track::Track(TrackList &trackList) : trackList(trackList) {}

Track::~Track() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

juce::uint64 Track::getTotalLength() const { return getTotalLengthSeconds() * sampleRate; }

double Track::getTotalLengthSeconds() const {
    double len = 0;
    for (std::shared_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->getSource() != nullptr) {
            len = std::max(len, sample->getEndPos());
        }
    }
    return len;
}

double Track::getSampleRate() const { return source == mixerSource ? 0.0 : sampleRate; }

bool Track::isSilenced() const { return muted || (!trackList.getSoloed().empty() && !soloed); }

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
    gainSource = std::make_shared<GainAudioSource>(newSource.get(), false);
    meteredSource = std::make_shared<MeteredAudioSource>(*gainSource, sampleRate);
}

void Track::loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    if (samples.size() == 0) {
        return;
    }
    mixerSource = std::make_shared<PositionableMixingAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    for (std::shared_ptr<Sample> &sample : samples) {
        sample->loadFile(formatManager);
        if (sample->getSource() != nullptr) {
            mixerSource->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
        }
    }
    setSource(mixerSource, deviceManager.getAudioDeviceSetup().sampleRate);
}

Sample *Track::addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
    juce::File file, double startPos, double endPos, double length, double sampleRate) {
    samples.push_back(std::make_shared<Sample>(file, startPos, endPos, length, sampleRate));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager);
    if (mixerSource == nullptr) {
        mixerSource = std::make_shared<PositionableMixingAudioSource>(deviceManager.getAudioDeviceSetup().sampleRate);
    }
    if (sample->getSource() != nullptr) {
        mixerSource->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
    }
    setSource(mixerSource, deviceManager.getAudioDeviceSetup().sampleRate);
    if (name == "") {
        name = file.getFileName();
    }
    return &(*samples.back());
}

void Track::moveSampleTo(Sample &sample, Track &toTrack) {
    for (auto iter = samples.begin(); iter != samples.end();) {
        if (&sample == iter->get()) {
            toTrack.samples.push_back(*iter);
            samples.erase(iter++);
        } else {
            ++iter;
        }
    }
}

void Track::adjustSampleLengthSecs(double newLen) {
    if (!deleted) {
        eachSample([newLen](Sample &sample) { sample.setMinLengthSecs(newLen); });
    }
}

Sample *Track::getSelected() const {
    for (std::shared_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->isSelected()) {
            return sample.get();
        }
    }
    return nullptr;
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    for (std::shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
    }
}

void Track::setLevelGain(float newLevel) {
    level = newLevel;
    if (gainSource != nullptr) {
        gainSource->setGain(level);
    }
}

void Track::setMute(bool newMuted) {
    DBG("Track " << number << " mute: " << (newMuted ? "true" : "false"));
    muted = newMuted;
    updateGain();
}

void Track::setSolo(bool newSoloed) {
    DBG("Track " << number << " solo: " << (newSoloed ? "true" : "false"));
    soloed = newSoloed;
    trackList.soloTracks();
}

void Track::updateGain() {
    if (gainSource != nullptr) {
        bool play = (trackList.getSoloed().size() == 0 || soloed) && !muted;
        DBG("Track " << number << " set gain: " << (play ? level : 0));
        gainSource->setGain(play ? level : 0);
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
    mixerSource->removeInputSource(sample->getSource());
}

void Track::undeleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(false);
    mixerSource->addInputSource(sample->getSource(), false, sample->getSampleRate(), 2);
}
