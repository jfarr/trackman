#include "TrackList.h"

TrackList::TrackList() {}

TrackList::~TrackList() {}

Track *TrackList::addTrack() {
    Track *track = new Track(*this);
    tracks.push_back(std::unique_ptr<Track>(track));
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
        } else
            ++iter;
    }
}

Track *TrackList::getSelected() const {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            return track.get();
        }
    }
    return nullptr;
}

juce::uint64 TrackList::getTotalLength() const {
    juce::uint64 total = 0;
    for (auto iter = tracks.begin(); iter != tracks.end(); iter++) {
        if (!(*iter)->isDeleted()) {
            juce::uint64 length = (*iter)->getTotalLength();
            total = std::max(total, length);
        }
    }
    return total;
}

double TrackList::getTotalLengthSeconds() const {
    double total = 0;
    for (auto iter = tracks.begin(); iter != tracks.end(); iter++) {
        if (!(*iter)->isDeleted()) {
            double length = (*iter)->getTotalLengthSeconds();
            total = std::max(total, length);
        }
    }
    return total;
}

void TrackList::adjustTrackLengths() {
    auto len = getTotalLengthSeconds();
    DBG("TrackList::adjustTrackLengths: " << len);
    eachTrack([len](Track &track) { track.adjustSampleLengthSecs(len); });
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
            track->setNumber(i++);
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

void TrackList::writeAudioFile(juce::File file, juce::AudioSource &source, double sampleRate, int bitsPerSample) {
    file.deleteFile();
    if (auto fileStream = std::unique_ptr<juce::FileOutputStream> (file.createOutputStream())) {
        juce::WavAudioFormat wavFormat;
        if (auto writer = wavFormat.createWriterFor (fileStream.get(), sampleRate, 2, bitsPerSample, {}, 0)) {
            fileStream.release();
            writer->writeFromAudioSource(source, getTotalLength());
            writer->flush();
            delete writer;
        }
    }
}
