#include "TrackList.h"
//#include "Track.h"
//#include "Mixer.h"

Track *TrackList::addTrack() {
    auto *track = new Track();
    tracks.push_back(std::unique_ptr<Track>(track));
    renumber();
    return track;
}
//
//Track *TrackList::createTempTrack() {
//    return new Track(*this, deviceManager);
//}

void TrackList::deleteTrack(Track *track) {
    if (track == nullptr) {
        return;
    }
    track->setDeleted(true);
//    if (track->getSource() != nullptr) {
//        mixer.removeSource(track->getSource());
//    }
    renumber();
}

void TrackList::undeleteTrack(Track *track) {
    if (track == nullptr) {
        return;
    }
    track->setDeleted(false);
//    if (track->getSource() != nullptr) {
//        mixer.addSource(track->getSource());
//    }
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

Sample *TrackList::addSample(Track &track, const juce::File &file, double startPos, double endPos,
    juce::AudioFormatManager &formatManager, double sampleRate) {
    return track.addSample(file, startPos, endPos, formatManager, sampleRate);
}

Track *TrackList::getSelectedTrack() const {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            return track.get();
        }
    }
    return nullptr;
}

double TrackList::getTotalLengthSeconds() const {
    return totalLengthSecs;
}

//void TrackList::updateLength() {
//    double newLen = 0;
//    for (const auto &track : tracks) {
//        if (!track->isDeleted()) {
//            newLen = std::max(newLen, track->getTotalLengthSeconds());
//        }
//    }
//    totalLengthSecs = newLen;
//    mixer.setTotalLengthSecs(totalLengthSecs);
//}

//void TrackList::updateAudioSources() {
//    mixer.removeAllSources();
//    for (std::unique_ptr<Track> &track : tracks) {
//        DBG("TrackList::updateAudioSources - add track source: " << track->getName());
////        if (track->getSource() != nullptr) {
////            mixer.addSource(track->getSource());
////        }
//    }
//}

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
            writer->writeFromAudioSource(source, (int)(getTotalLengthSeconds() * sampleRate));
            writer->flush();
            delete writer;
        }
    }
}
