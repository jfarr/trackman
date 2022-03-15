#include "Track.h"

#include <memory>
#include "TrackList.h"

Track::Track() {}

Track::~Track() { samples.clear(); }

void Track::setLevelGain(float newLevel) {
    level = newLevel;
    if (gainSource != nullptr) {
        gainSource->setGain(level);
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

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
}

void Track::selectSample(Sample *newSelected) {
    eachSample([&newSelected](Sample &sample) { sample.setSelected(&sample == newSelected); });
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
//
//void Track::deleteSample(Sample *sample) {
//    if (sample == nullptr) {
//        return;
//    }
//    sample->setDeleted(true);
//}
//
//void Track::undeleteSample(Sample *sample) {
//    if (sample == nullptr) {
//        return;
//    }
//    sample->setDeleted(false);
//}

Sample *Track::addSample(const juce::File &file, double startPos, double endPos,
    juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    samples.push_back(std::make_shared<Sample>(file, startPos, endPos));
    auto sample = &(*samples.back());
    sample->loadFile(deviceManager, formatManager);
    if (samplePlayer == nullptr) {
        samplePlayer = std::make_unique<SamplePlayer>(samples);
        gainSource = std::make_unique<GainAudioSource>(samplePlayer.get(), false);
        meteredSource = std::make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);
    }
    return &(*samples.back());
}

void Track::setMute(bool newMuted) {
    muted = newMuted;
}

void Track::setSolo(bool newSoloed) {
    soloed = newSoloed;
}

void Track::updateGain(bool anySoloed) {
    bool play = (!anySoloed || soloed) && !muted;
    if (gainSource != nullptr) {
        gainSource->setGain(play ? level : 0);
    }
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    for (std::shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
    }
}

juce::int64 Track::getTotalLengthInSamples() const { return meteredSource == nullptr ? 0 : meteredSource->getTotalLength(); }
