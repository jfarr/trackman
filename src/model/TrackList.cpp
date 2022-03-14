#include "TrackList.h"
#include "Mixer.h"

Track *TrackList::addTrack() {
    auto *track = new Track(*this, deviceManager);
    tracks.push_back(std::unique_ptr<Track>(track));
    renumber();
    return track;
}

Track *TrackList::createTempTrack() {
    return new Track(*this, deviceManager);
}

void TrackList::deleteTrack(Track *track) {
    if (track == nullptr) {
        return;
    }
    track->setDeleted(true);
    renumber();
}

void TrackList::undeleteTrack(Track *track) {
    if (track == nullptr) {
        return;
    }
    track->setDeleted(false);
    renumber();
}

void TrackList::removeTrack(Track *track) {
    for (auto iter = tracks.begin(); iter != tracks.end();) {
        if (track == iter->get()) {
            tracks.erase(iter++);
        } else {
            ++iter;
        }
    }
}

Track *TrackList::getSelectedTrack() const {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            return track.get();
        }
    }
    return nullptr;
}
//
//juce::uint64 TrackList::getTotalLength() const {
//    juce::uint64 total = 0;
//    for (const auto &track : tracks) {
//        if (!track->isDeleted()) {
//            juce::uint64 length = track->getTotalLength();
//            total = std::max(total, length);
//        }
//    }
//    return total;
//}

double TrackList::getTotalLengthSeconds() const {
    return totalLengthSecs;
    //    double total = 0;
    //    for (const auto & track : tracks) {
    //        if (!track->isDeleted()) {
    //            double length = track->getTotalLengthSeconds();
    //            total = std::max(total, length);
    //        }
    //    }
    //    return total;
}

//void TrackList::adjustTrackLengths() {
//    //    auto len = getTotalLengthSeconds();
//    //    DBG("TrackList::adjustTrackLengths: " << len);
//    //    eachTrack([len](Track &track) {
//    //        DBG("adjusting length for track " << track.getName() << " to " << len);
//    //        track.adjustSampleLengthSecs(len);
//    //    });
//}

void TrackList::updateLength() {
    double newLen = 0;
    for (const auto &track : tracks) {
        if (!track->isDeleted()) {
            newLen = std::max(newLen, track->getTotalLengthSeconds());
        }
    }
    totalLengthSecs = newLen;
    mixer.setTotalLengthSecs(totalLengthSecs);
    for (const auto &track : tracks) {
        if (!track->isDeleted()) {
            track->setTotalLengthSecs(newLen);
        }
    }
}

void TrackList::eachTrack(std::function<void(Track &track)> f) {
    for (std::unique_ptr<Track> &track : tracks) {
        if (!track->isDeleted()) {
            f(*track);
        }
    }
}

void TrackList::renumber() {
    int i = 1;
    for (std::unique_ptr<Track> &track : tracks) {
        if (!track->isDeleted()) {
            track->setTrackNumber(i++);
        }
    }
}

void TrackList::setSelected(Track *selected) {
    eachTrack([&selected](Track &track) { track.setSelected(&track == selected); });
}

void TrackList::selectSample(Sample *selected) {
    eachTrack([&selected](Track &track) { track.selectSample(selected); });
}

Sample *TrackList::getSelectedSample() const {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            Sample *selected = track->getSelected();
            if (selected != nullptr) {
                return selected;
            }
        }
    }
    return nullptr;
}

void TrackList::soloTracks() {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted()) {
            track->updateGain();
        }
    }
}

std::list<const Track *> TrackList::getSoloed() {
    std::list<const Track *> soloed;
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSoloed()) {
            soloed.push_back(&*track);
        }
    }
    return soloed;
}

void TrackList::writeAudioFile(
    const juce::File &file, juce::AudioSource &source, double sampleRate, int bitsPerSample) const {
    file.deleteFile();
    if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream())) {
        juce::WavAudioFormat wavFormat;
        if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 2, bitsPerSample, {}, 0)) {
            fileStream.release();
            writer->writeFromAudioSource(source, (int)(getTotalLengthSeconds() * deviceManager.getAudioDeviceSetup().sampleRate));
            writer->flush();
            delete writer;
        }
    }
}
