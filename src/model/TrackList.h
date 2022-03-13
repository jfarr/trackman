#pragma once

#include "Track.h"

class Mixer;

class TrackList {
  public:
    TrackList(Mixer &mixer, double sampleRate) : mixer(mixer), sampleRate(sampleRate) {}
    ~TrackList() = default;

    Track *addTrack();
    void deleteTrack(Track *track);
    void undeleteTrack(Track *track);
    void removeTrack(Track *track);

    [[nodiscard]] int size() const { return (int)tracks.size(); }
    [[nodiscard]] Track *getSelectedTrack() const;
    [[nodiscard]] Sample *getSelectedSample() const;
    [[nodiscard]] juce::uint64 getTotalLength() const;
    [[nodiscard]] double getTotalLengthSeconds() const;
    [[nodiscard]] double getSampleRate() const { return sampleRate; }

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
//    void adjustTrackLengths();
    void setSelected(Track *selected);
    void selectSample(Sample *selected);

    void soloTracks();
    std::list<const Track *> getSoloed();

    void writeAudioFile(const juce::File &file, juce::AudioSource &source, double sampleRate, int bitsPerSample) const;

  private:
    friend Track;

    void updateLength();

    Mixer &mixer;
    double sampleRate;
    std::list<std::unique_ptr<Track>> tracks;
    double totalLengthSecs = 0;

    void renumber();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
