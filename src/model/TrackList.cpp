#include "TrackList.h"

TrackList::TrackList() {}

TrackList::~TrackList() {
    for (std::list<Track *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        std::unique_ptr<Track> toDelete;
        toDelete.reset(*i);
    }
}

Track *TrackList::addTrack(juce::String name) {
    Track *track = new Track(name);
    tracks.push_back(track);
    return track;
}

void TrackList::eachTrack(std::function<void(Track &)> f) {
    for (std::list<Track *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        f(**i);
    }
}
