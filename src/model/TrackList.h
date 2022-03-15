#pragma once

#include <JuceHeader.h>

#include "Track.h"

// class Mixer;
// class Track;
// class Sample;
class Project;

class TrackList {
  public:
    TrackList(juce::AudioDeviceManager &deviceManager) : deviceManager(deviceManager) {}
    ~TrackList() = default;

    Track *addTrack();
    //    Track *createTempTrack();
    void undeleteTrack(Track *track);
    void removeTrack(Track *track);

    [[nodiscard]] int size() const { return (int)tracks.size(); }
    [[nodiscard]] Track *getSelectedTrack() const;
    [[nodiscard]] Sample *getSelectedSample() const;
    [[nodiscard]] double getTotalLengthInSeconds() const;
    [[nodiscard]] juce::int64 getTotalLengthInSamples() const;

    void eachTrack(std::function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void setSelected(Track *selected);
    void selectSample(Sample *selected);

    void soloTracks();
    std::list<const Track *> getSoloed();

    void writeAudioFile(const juce::File &file, juce::AudioSource &source, double sampleRate, int bitsPerSample) const;

  private:
    //    friend Track;
    //    friend Mixer;
    friend Project;

    void deleteTrack(Track *track);
    Sample *addSample(
        Track &track, const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager);

    //    void updateLength();
    //    void updateAudioSources();

    //    Mixer &mixer;
    juce::AudioDeviceManager &deviceManager;
    std::list<std::unique_ptr<Track>> tracks;
    //    double totalLengthSecs = 0;

    void renumber();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};
