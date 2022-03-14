#pragma once

#include "Track.h"

class Mixer;

class TrackList {
  public:
    TrackList(Mixer &mixer, juce::AudioDeviceManager &deviceManager) : mixer(mixer), deviceManager(deviceManager) {}
    ~TrackList() = default;

    Track *addTrack();
    Track *createTempTrack();
    void deleteTrack(Track *track);
    void undeleteTrack(Track *track);
    void removeTrack(Track *track);

    [[nodiscard]] int size() const { return (int)tracks.size(); }
    [[nodiscard]] Track *getSelectedTrack() const;
    [[nodiscard]] Sample *getSelectedSample() const;
    [[nodiscard]] double getTotalLengthSeconds() const;

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void setSelected(Track *selected);
    void selectSample(Sample *selected);

    void soloTracks();
    std::list<const Track *> getSoloed();

    void writeAudioFile(const juce::File &file, juce::AudioSource &source, double sampleRate, int bitsPerSample) const;

  private:
    friend Track;
    friend Mixer;

    void updateLength();
    void updateAudioSources();

    Mixer &mixer;
    juce::AudioDeviceManager &deviceManager;
    std::list<std::unique_ptr<Track>> tracks;
    double totalLengthSecs = 0;

    void renumber();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
