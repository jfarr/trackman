#include "TrackList.h"

TrackList::TrackList() {}

TrackList::~TrackList() {}

Track *TrackList::addTrack(juce::String name) {
    Track *track = new Track(name);
    tracks.push_back(std::unique_ptr<Track>(track));
    return track;
}

void TrackList::removeTrack(Track *track) {
    for (auto iter = tracks.begin(); iter != tracks.end();) {
        if (track == iter->get()) {
            tracks.erase(iter++);
        } else
            ++iter;
    }
}

bool TrackList::hasSelection() const {
    for (auto iter = tracks.begin(); iter != tracks.end(); iter++) {
        if ((*iter)->isSelected() && !(*iter)->isDeleted()) {
            return true;
        }
    }
    return false;
}

Track *TrackList::getSelected() const {
    for (std::unique_ptr<Track> const &track : tracks) {
        if (!track->isDeleted() && track->isSelected()) {
            return track.get();
        }
    }
    return nullptr;
}

double TrackList::getTotalLengthSeconds() const {
    double total = 0;
    for (auto iter = tracks.begin(); iter != tracks.end(); iter++) {
        double length = (*iter)->getTotalLengthSeconds();
        total = std::max(total, length);
    }
    return total;
}

void TrackList::eachTrack(std::function<void(Track &track)> f) {
    for (std::unique_ptr<Track> &track : tracks) {
        if (!track->isDeleted()) {
            f(*track);
        }
    }
}

void TrackList::setSelected(Track &selected) {
    eachTrack([&selected](Track &track) { track.setSelected(&track == &selected); });
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