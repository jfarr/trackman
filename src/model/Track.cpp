#include "Track.h"
#include "TrackList.h"

Track::Track() {}

Track::~Track() { samples.clear(); }

// double Track::getTotalLengthSeconds() const { return totalLengthSecs; }

// bool Track::isSilenced() const { return muted || (!trackList.getSoloed().empty() && !soloed); }

//void Track::loadSamples(juce::AudioFormatManager &formatManager, double sampleRate) {
//    //    if (samples.empty()) {
//    //        return;
//    //    }
//    //    if (mixerSource == nullptr) {
//    //        mixerSource = std::make_shared<PositionableMixingAudioSource>(deviceManager);
//    //    }
//    for (std::shared_ptr<Sample> &sample : samples) {
//        sample->loadFile(formatManager, sampleRate);
//        //        if (sample->getSource() != nullptr) {
//        //            mixerSource->addInputSource(sample->getSource());
//        //        }
//    }
//    if (samplePlayer == nullptr) {
//        samplePlayer.reset(new SamplePlayer(samples));
//    }
//
//    //    setSource(mixerSource);
//    //    updateLength();
//}

Sample *Track::addSample(const juce::File &file, double startPos, double endPos,
    juce::AudioFormatManager &formatManager, double sampleRate) {
    samples.push_back(std::make_shared<Sample>(file, startPos, endPos));
    auto sample = &(*samples.back());
    sample->loadFile(formatManager, sampleRate);
    if (samplePlayer == nullptr) {
        samplePlayer.reset(new SamplePlayer(samples));
    }

    //    if (mixerSource == nullptr) {
    //        mixerSource = std::make_shared<PositionableMixingAudioSource>(deviceManager);
    //    }
    //    if (sample->getSource() != nullptr) {
    //        mixerSource->addInputSource(sample->getSource());
    //    }
    //    setSource(mixerSource);
    //    if (name == defaultName) {
    //        name = file.getFileName();
    //    }
    //    updateLength();
    //    trackList.updateAudioSources();
    return &(*samples.back());
}

//
// void Track::setSource(std::shared_ptr<juce::PositionableAudioSource> newSource) {
//    DBG("Track::setSource - set track source: " << getName());
//    if (source == newSource) {
//        return;
//    }
//    if (source != nullptr) {
//        source->releaseResources();
//    }
//    source = newSource;
//    gainSource = std::make_shared<GainAudioSource>(newSource.get(), false);
//    meteredSource = std::make_shared<MeteredAudioSource>(*gainSource, deviceManager.getAudioDeviceSetup().sampleRate);
//}

void Track::moveSampleTo(Sample &sample, Track &toTrack) {
    for (auto iter = samples.begin(); iter != samples.end();) {
        if (&sample == iter->get()) {
            //            sample.setTrack(toTrack);
            //            auto source = sample.getSource();
            //            if (source != nullptr) {
            //                if (mixerSource != nullptr) {
            //                    mixerSource->removeInputSource(source);
            //                }
            //                if (toTrack.mixerSource != nullptr) {
            //                    toTrack.mixerSource->addInputSource(source);
            //                }
            //            }
            toTrack.samples.push_back(*iter);
            samples.erase(iter++);
        } else {
            ++iter;
        }
    }
    //    updateLength();
    //    toTrack.updateLength();
}

void Track::deleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(true);
    //    auto source = sample->getSource();
    //    if (source != nullptr && mixerSource != nullptr) {
    //        mixerSource->removeInputSource(source);
    //    }
    //    updateLength();
}

void Track::undeleteSample(Sample *sample) {
    if (sample == nullptr) {
        return;
    }
    sample->setDeleted(false);
    //    auto source = sample->getSource();
    //    if (source != nullptr && mixerSource != nullptr) {
    //        mixerSource->addInputSource(source);
    //        mixerSource->prepareToPlay(
    //            deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    //    }
    //    updateLength();
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
    //    if (deleted) {
    //        for (std::shared_ptr<Sample> &sample : samples) {
    //            if (sample->getSource() != nullptr && mixerSource != nullptr) {
    //                mixerSource->removeInputSource(sample->getSource());
    //            }
    //        }
    //    }
}
//
// void Track::updateLength() {
//    double newLen = 0;
//    for (std::shared_ptr<Sample> &sample : samples) {
//        if (!(*sample).isDeleted()) {
//            newLen = std::max(newLen, sample->getEndPos());
//        }
//    }
//    setTotalLengthSecs(newLen);
//    trackList.updateLength();
//}
//
// void Track::setTotalLengthSecs(double newLen) {
//    totalLengthSecs = newLen;
//    if (mixerSource != nullptr) {
//        mixerSource->setTotalLengthSecs(newLen);
//    }
//}

void Track::setLevelGain(float newLevel) {
    level = newLevel;
    //    if (gainSource != nullptr) {
    //        gainSource->setGain(level);
    //    }
}

void Track::setMute(bool newMuted) {
    muted = newMuted;
    updateGain();
}

void Track::setSolo(bool newSoloed) {
    soloed = newSoloed;
    //    trackList.soloTracks();
}

void Track::updateGain() {
    //    bool play = (trackList.getSoloed().empty() || soloed) && !muted;
    //    if (gainSource != nullptr) {
    //        gainSource->setGain(play ? level : 0);
    //    }
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
