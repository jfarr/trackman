#pragma once

#include "Mixer.h"
#include "TrackList.h"

class Project {
  public:
    Project(TrackList &trackList, Mixer &mixer) : trackList(trackList), mixer(mixer) {}
    ~Project() {}

    std::string to_json();
    void from_json(juce::AudioFormatManager &formatManager, std::string filename);

  private:
    TrackList &trackList;
    Mixer &mixer;
};
