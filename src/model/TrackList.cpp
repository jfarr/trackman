#include "TrackList.h"

namespace trackman {

Track *TrackList::addTrack() {
    auto *track = new Track(project, deviceManager);
    tracks.push_back(unique_ptr<Track>(track));
    renumber();
    return track;
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

Sample *TrackList::addSample(
    Track &track, const File &file, double startPos, double endPos, AudioFormatManager &formatManager) {
    return track.addSample(file, startPos, endPos, formatManager);
}

Track *TrackList::getSelectedTrack() const {
    for (unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            return track.get();
        }
    }
    return nullptr;
}

double TrackList::getTotalLengthInSeconds() const {
    return getTotalLengthInSamples() / deviceManager.getAudioDeviceSetup().sampleRate;
}

int64 TrackList::getTotalLengthInSamples() const {
    int64 length = 0;
    for (const unique_ptr<Track> &track : tracks) {
        if (!track->isDeleted()) {
            length = max(length, track->getTotalLengthInSamples());
        }
    }
    return length;
}

void TrackList::eachTrack(function<void(Track &track)> f) {
    for (unique_ptr<Track> &track : tracks) {
        if (!track->isDeleted()) {
            f(*track);
        }
    }
}

void TrackList::renumber() {
    int i = 1;
    for (unique_ptr<Track> &track : tracks) {
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
    for (unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            Sample *selected = track->getSelected();
            if (selected != nullptr) {
                return selected;
            }
        }
    }
    return nullptr;
}

void TrackList::setMute(Track &track, bool newMuted) {
    track.setMute(newMuted);
    track.updateGain(isAnySoloed());
}

void TrackList::setSolo(Track &track, bool newSoloed) {
    track.setSolo(newSoloed);
    auto anySoloed = isAnySoloed();
    for (unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted()) {
            track->updateGain(anySoloed);
        }
    }
}

bool TrackList::isAnySoloed() const {
    for (unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSoloed()) {
            return true;
        }
    }
    return false;
}

MidiRecorder *TrackList::getMidiRecorder() {
    auto selected = getSelectedTrack();
    return selected == nullptr ? nullptr : selected->getMidiRecorder();
}

void TrackList::writeAudioFile(const File &file, AudioSource &source, double sampleRate, int bitsPerSample) const {
    file.deleteFile();
    if (auto fileStream = unique_ptr<FileOutputStream>(file.createOutputStream())) {
        WavAudioFormat wavFormat;
        if (auto writer = wavFormat.createWriterFor(fileStream.get(), sampleRate, 2, bitsPerSample, {}, 0)) {
            fileStream.release();
            writer->writeFromAudioSource(source, (int)(getTotalLengthInSamples()));
            writer->flush();
            delete writer;
        }
    }
}

} // namespace trackman
