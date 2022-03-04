#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    Track *addTrack(juce::String name);
//    void removeTrack(Track *track);
    int size() { return tracks.size(); }

    void setSelected(Track &selected);

    void eachTrack(std::function<void(Track &track)> f);

  private:
    std::list<std::unique_ptr<Track>> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
