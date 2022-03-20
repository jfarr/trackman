#pragma once

#include "Mixer.h"
#include "TrackList.h"
#include "TimeSignature.h"

class Project {
  public:
    Project(juce::AudioDeviceManager &deviceManager, MidiRecorder &midiRecorder);
    ~Project() = default;

    const float getTempo() const { return tempo; };
    void setTempo(const float newTempo) { tempo = newTempo; }

    Track *addTrack();
    void deleteTrack(Track *track);

    Sample *addSample(
        Track &track, const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager);

    TrackList &getTrackList() { return trackList; }
    Mixer &getMixer() { return mixer; }

    Track *getSelectedTrack() const { return trackList.getSelectedTrack(); }
    Sample *getSelectedSample() const { return trackList.getSelectedSample(); }

    int secondsToTicks(double seconds);
    double ticksToSeconds(int ticks);
    double measuresToSeconds(double measures);

    double getVerticalScale() const { return verticalScale; }
    double getHorizontalScale() const { return horizontalScale; }
//    double getHorizontalScaleRatio() const { return initialHorizontalScale / horizontalScale; }

    void incrementVerticalScale() { verticalScale *= 1.08; }
    void decrementVerticalScale() {
        verticalScale /= 1.08;
        verticalScale = std::max(verticalScale, 0.0000001);
    }
    void incrementHorizontalScale() { horizontalScale *= 1.2; }
    void decrementHorizontalScale() {
        horizontalScale /= 1.2;
        horizontalScale = std::max(horizontalScale, 0.0000001);
    }

    std::string to_json();
    void from_json(juce::AudioFormatManager &formatManager, std::string filename);

    void writeAudioFile(const juce::File &file);

  private:
    const float initialTempo = 120.0;
    const double initialVerticalScale = 1;
    const double initialHorizontalScale = 100;
    const double scaleIncrement = 5;

    juce::AudioDeviceManager &deviceManager;
    TrackList trackList;
    Mixer mixer;
    TimeSignature timeSignature;
    float tempo = initialTempo;
    double verticalScale = initialVerticalScale;
    double horizontalScale = initialHorizontalScale;
};
