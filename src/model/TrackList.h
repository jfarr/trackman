#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    int size() { return tracks.size(); }
    Track* addTrack(juce::String name);

    void eachTrack(std::function<void(Track&)> f);

  private:
    std::list<Track *> tracks;
};
