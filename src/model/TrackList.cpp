#include "TrackList.h"

TrackList::TrackList() {}

TrackList::~TrackList() {
    for (Track *track : tracks) {
        delete track;
    }
    tracks.clear();
}

Track *TrackList::addTrack(juce::String name) {
    Track *track = new Track(name);
    tracks.push_back(track);
    return track;
}

void TrackList::removeTrack(Track *track) {
    tracks.remove(track);
    delete track;
}
