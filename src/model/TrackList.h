#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    int size() { return tracks.size(); }
    Track *addTrack(juce::String name);

    void eachTrack(std::function<void(Track &)> f);

  private:
    std::list<Track *> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
