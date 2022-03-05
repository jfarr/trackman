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

juce::int64 TrackList::getTotalLength() const {
    juce::int64 total = 0;
    for (auto iter = tracks.begin(); iter != tracks.end(); iter++) {
        auto length = (*iter)->getTotalLength();
        total = std::max(total, length);
    }
    return total;
}

void TrackList::setSelected(Track &selected) {
    eachTrack([&selected](Track &track) { track.setSelected(&track == &selected); });
}

void TrackList::eachTrack(std::function<void(Track &track)> f) {
    std::for_each(tracks.begin(), tracks.end(), [&f](std::unique_ptr<Track> &track) {
        if (!track->isDeleted()) {
            f(*track);
        }
    });
}
