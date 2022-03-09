#pragma once

#include "Track.h"

class TrackList {
  public:
    TrackList();
    ~TrackList();

    Track *addTrack();
    void deleteTrack(Track *track);
    void undeleteTrack(Track *track);
    void removeTrack(Track *track);

    int size() const { return tracks.size(); }
    Track *getSelected() const;
    Sample *getSelectedSample() const;
    double getTotalLengthSeconds() const;

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void adjustTrackLengths();
    void setSelected(Track *selected);
    void selectSample(Sample *selected);

    void soloTrack(Track *soloed);
    void unsoloTrack(Track *soloed);
    void soloTracks();
    std::list<const Track *> getSoloed();

  private:
    std::list<std::unique_ptr<Track>> tracks;

    void renumber();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
