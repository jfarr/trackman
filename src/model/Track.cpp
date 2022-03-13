#include "Track.h"
#include "TrackList.h"
#include "audio/PositionableMixingAudioSource.h"

Track::Track(TrackList &trackList)
    : trackList(trackList), mixerSource(), gainSource(&mixerSource, false),
      meteredSource(gainSource, trackList.getSampleRate()) {}

juce::uint64 Track::getTotalLength() const {
    return (juce::uint64)getTotalLengthSeconds() * (juce::uint64)trackList.getSampleRate();
}

double Track::getTotalLengthSeconds() const {
    double len = 0;
    for (std::shared_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->getSource() != nullptr) {
            len = std::max(len, sample->getEndPos());
        }
    }
    return len;
}

double Track::getSampleRate() const { return trackList.getSampleRate(); }

bool Track::isSilenced() const { return muted || (!trackList.getSoloed().empty() && !soloed); }

void Track::loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    if (samples.empty()) {
        return;
    }
    for (std::shared_ptr<Sample> &sample : samples) {
        sample->loadFile(formatManager, deviceManager.getAudioDeviceSetup().sampleRate);
        if (sample->getSource() != nullptr) {
            mixerSource.addInputSource(*sample->getSource());
        }
    }
}

Sample *Track::addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
    const juce::File &file, double startPos, double endPos, double length, double fileSampleRate) {
    samples.push_back(std::make_shared<Sample>(file, startPos, endPos, length, fileSampleRate));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager, deviceManager.getAudioDeviceSetup().sampleRate);
    if (sample->getSource() != nullptr) {
        mixerSource.addInputSource(*sample->getSource());
    }
    if (name == defaultName) {
        name = file.getFileName();
    }
    return &(*samples.back());
}

void Track::moveSampleTo(Sample &sample, Track &toTrack) {
    for (auto iter = samples.begin(); iter != samples.end();) {
        if (&sample == iter->get()) {
            auto source = sample.getSource();
            if (source != nullptr) {
                mixerSource.removeInputSource(*source);
                toTrack.mixerSource.addInputSource(*source);
            }
            toTrack.samples.push_back(*iter);
            samples.erase(iter++);
        } else {
            ++iter;
        }
    }
    trackList.adjustTrackLengths();
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
    gainSource.setGain(level);
}

void Track::setMute(bool newMuted) {
    DBG("Track " << trackNumber << " mute: " << (newMuted ? "true" : "false"));
    muted = newMuted;
    updateGain();
}

void Track::setSolo(bool newSoloed) {
    DBG("Track " << trackNumber << " solo: " << (newSoloed ? "true" : "false"));
    soloed = newSoloed;
    trackList.soloTracks();
}

void Track::updateGain() {
    bool play = (trackList.getSoloed().empty() || soloed) && !muted;
    DBG("Track " << trackNumber << " set gain: " << (play ? level : 0));
    gainSource.setGain(play ? level : 0);
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
    if (deleted) {
        for (std::shared_ptr<Sample> &sample : samples) {
            if (sample->getSource() != nullptr) {
                mixerSource.removeInputSource(*sample->getSource());
            }
        }
    }
}

void Track::selectSample(Sample *newSelected) {
    eachSample([&newSelected](Sample &sample) { sample.setSelected(&sample == newSelected); });
}

void Track::deleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(true);
    mixerSource.removeInputSource(*sample->getSource());
}

void Track::undeleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(false);
    mixerSource.addInputSource(*sample->getSource());
}
