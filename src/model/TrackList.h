#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    Track *addTrack(juce::String name);
    void removeTrack(Track *track);

    int size() const { return tracks.size(); }
    bool hasSelection() const;
    double getTotalLengthSeconds() const;

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void setSelected(Track &selected);
    void selectSample(Sample &selected);

  private:
    std::list<std::unique_ptr<Track>> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
