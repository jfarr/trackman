#include "Track.h"
#include "TrackList.h"
#include "audio/PositionableMixingAudioSource.h"

Track::Track(TrackList &trackList, juce::AudioDeviceManager &deviceManager)
    : trackList(trackList), mixerSource(deviceManager), gainSource(&mixerSource, false) {
    meteredSource = std::make_unique<MeteredAudioSource>(gainSource, deviceManager.getAudioDeviceSetup().sampleRate);
}

//juce::uint64 Track::getTotalLength() const {
//    return (juce::uint64)getTotalLengthSeconds() * (juce::uint64)trackList.getSampleRate();
//}

double Track::getTotalLengthSeconds() const {
    //    double len = 0;
    //    for (std::shared_ptr<Sample> const &sample : samples) {
    //        if (!sample->isDeleted() && sample->getSource() != nullptr) {
    //            len = std::max(len, sample->getEndPos());
    //        }
    //    }
    //    DBG("track len secs: " << len);
    //    return len;
    DBG("track len secs: " << totalLengthSecs);
    return totalLengthSecs;
}

// double Track::getSampleRate() const { return trackList.getSampleRate(); }

bool Track::isSilenced() const { return muted || (!trackList.getSoloed().empty() && !soloed); }

void Track::loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    if (samples.empty()) {
        return;
    }
    for (std::shared_ptr<Sample> &sample : samples) {
        sample->loadFile(formatManager, deviceManager.getAudioDeviceSetup().sampleRate);
        if (sample->getSource() != nullptr) {
            mixerSource.addInputSource(sample->getSource());
        }
    }
    updateLength();
}

Sample *Track::addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
    const juce::File &file, double startPos, double endPos, double length, double fileSampleRate) {
    samples.push_back(std::make_shared<Sample>(*this, file, startPos, endPos, length, fileSampleRate));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager, deviceManager.getAudioDeviceSetup().sampleRate);
    if (sample->getSource() != nullptr) {
        mixerSource.addInputSource(sample->getSource());
    }
    if (name == defaultName) {
        name = file.getFileName();
    }
    updateLength();
    return &(*samples.back());
}

void Track::moveSampleTo(Sample &sample, Track &toTrack) {
    for (auto iter = samples.begin(); iter != samples.end();) {
        if (&sample == iter->get()) {
            auto source = sample.getSource();
            if (source != nullptr) {
                mixerSource.removeInputSource(source);
                toTrack.mixerSource.addInputSource(source);
            }
            toTrack.samples.push_back(*iter);
            samples.erase(iter++);
            updateLength();
            toTrack.updateLength();
        } else {
            ++iter;
        }
    }
    //    trackList.adjustTrackLengths();
}

void Track::deleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(true);
    auto source = sample->getSource();
    if (source != nullptr) {
        mixerSource.removeInputSource(source);
    }
    updateLength();
}

void Track::undeleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(false);
    auto source = sample->getSource();
    if (source != nullptr) {
        mixerSource.addInputSource(source);
    }
    updateLength();
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
    if (deleted) {
        for (std::shared_ptr<Sample> &sample : samples) {
            if (sample->getSource() != nullptr) {
                mixerSource.removeInputSource(sample->getSource());
            }
        }
    }
}

void Track::updateLength() {
    double newLen = 0;
    for (std::shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            newLen = std::max(newLen, sample->getEndPos());
        }
    }
    setTotalLengthSecs(newLen);
    trackList.updateLength();
}

void Track::setTotalLengthSecs(double newLen) {
    totalLengthSecs = newLen;
    mixerSource.setTotalLengthSecs(newLen);
}

// void Track::adjustSampleLengthSecs(double newLen) {
//     if (!deleted) {
//         eachSample([newLen](Sample &sample) { sample.setMinLengthSecs(newLen); });
//     }
// }

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

Sample *Track::getSelected() const {
    for (std::shared_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->isSelected()) {
            return sample.get();
        }
    }
    return nullptr;
}

void Track::selectSample(Sample *newSelected) {
    eachSample([&newSelected](Sample &sample) { sample.setSelected(&sample == newSelected); });
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    for (std::shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
    }
}
