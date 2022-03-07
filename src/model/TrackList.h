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
    Track *getSelected() const;
    Sample *getSelectedSample() const;
    juce::int64 getTotalLength() const;
    double getTotalLengthSeconds() const;

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void adjustTrackLengths();
    void setSelected(Track &selected);
    void selectSample(Sample *selected);

  private:
    std::list<std::unique_ptr<Track>> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
