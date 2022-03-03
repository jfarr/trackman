#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    Track *addTrack(juce::String name);
    void removeTrack(Track *track);
    int size() { return tracks.size(); }

    void eachTrack(std::function<void(Track &track)> f) {
        std::for_each(tracks.begin(), tracks.end(), [&f](Track *track) { f(*track); });
    }

  private:
    std::list<Track *> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
