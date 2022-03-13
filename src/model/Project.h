#pragma once

#include "Mixer.h"
#include "TrackList.h"

class Project {
  public:
    Project(double sampleRate) : mixer(sampleRate) {}
    ~Project() = default;

    TrackList &getTrackList() { return trackList; }
    Mixer &getMixer() { return mixer; }

    Track *getSelectedTrack() const { return trackList.getSelectedTrack(); }
    Sample *getSelectedSample() const { return trackList.getSelectedSample(); }

    double getVerticalScale() const { return verticalScale; }
    double getHorizontalScale() const { return horizontalScale; }

    void incrementVerticalScale() { verticalScale *= 1.05; }
    void decrementVerticalScale() {
        verticalScale /= 1.05;
        verticalScale = std::max(verticalScale, 0.0000001);
    }
    void incrementHorizontalScale() { horizontalScale *= 1.1; }
    void decrementHorizontalScale() {
        horizontalScale /= 1.1;
        horizontalScale = std::max(horizontalScale, 0.0000001);
    }

    std::string to_json();
    void from_json(
        juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, std::string filename);

  private:
    const double initialVerticalScale = 1;
    const double initialHorizontalScale = 100;
    const double scaleIncrement = 5;

    TrackList trackList;
    Mixer mixer;
    double verticalScale = initialVerticalScale;
    double horizontalScale = initialHorizontalScale;
};
