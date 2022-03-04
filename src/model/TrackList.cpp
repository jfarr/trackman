#include "TrackList.h"

TrackList::TrackList() {}

TrackList::~TrackList() {
}

Track *TrackList::addTrack(juce::String name) {
    Track *track = new Track(name);
    tracks.push_back(std::unique_ptr<Track>(track));
    return track;
}
//
//void TrackList::removeTrack(Track *track) {
//    tracks.remove(track);
//    delete track;
//}

void TrackList::setSelected(Track &selected) {
    eachTrack([&selected](Track& track) {
        track.setSelected(&track == &selected);
    });
}

void TrackList::eachTrack(std::function<void(Track &track)> f) {
    std::for_each(tracks.begin(), tracks.end(), [&f](std::unique_ptr<Track> &track) {
        if (!track->isDeleted()) {
            f(*track);
        }
    });
}
