#pragma once

#include "Mixer.h"
#include "TrackList.h"

class Project {
  public:
    Project(TrackList &trackList, Mixer &mixer) : trackList(trackList), mixer(mixer) {}
    ~Project() = default;

    double getHorizontalScale() const { return horizontalScale; }

    void setHorizontalScale(int newScale) { horizontalScale = newScale; }
    void incrementHorizontalScale() { horizontalScale *= 1.1; }
    void decrementHorizontalScale() { horizontalScale /= 1.1; horizontalScale = std::max(horizontalScale, 0.0000001); }

    std::string to_json();
    void from_json(
        juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, std::string filename);

  private:
    const double initialHorizontalScale = 75;
    const double scaleIncrement = 5;

    TrackList &trackList;
    Mixer &mixer;
    double horizontalScale = initialHorizontalScale;
};
