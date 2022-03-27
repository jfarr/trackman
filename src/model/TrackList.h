#pragma once

#include <JuceHeader.h>

#include "Track.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class TrackList {
  public:
    TrackList(Project &project, AudioDeviceManager &deviceManager)
        : project(project), deviceManager(deviceManager) {}
    ~TrackList() = default;

    void undeleteTrack(Track *track);
    void removeTrack(Track *track);

    int size() const { return (int)tracks.size(); }
    Track *getSelectedTrack() const;
    Sample *getSelectedSample() const;
    double getTotalLengthInSeconds() const;
    int64 getTotalLengthInSamples() const;

    void eachTrack(function<void(Track &track)> f);

    void clear() { tracks.clear(); }
    void setMute(Track &track, bool newMuted);
    void setSolo(Track &track, bool newSoloed);
    void setSelected(Track *selected);
    void selectSample(Sample *selected);

    bool canRecord() const { return getSelectedTrack() != nullptr && getSelectedTrack()->canRecord(); }

    void selectNoteRoll(NoteRoll *selected);

  private:
    friend Project;

    Track *addTrack();
    void deleteTrack(Track *track);
    Sample *addSample(
        Track &track, const File &file, double startPos, double endPos, AudioFormatManager &formatManager);

    Project &project;
    AudioDeviceManager &deviceManager;
    list<unique_ptr<Track>> tracks;

    void renumber();
    bool isAnySoloed() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackList)
};

}
