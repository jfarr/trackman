#pragma once

#include "InstrumentPlayer.h"
#include "Mixer.h"
#include "TimeSignature.h"
#include "TrackList.h"
#include "Transport.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project {
  public:
    Project(AudioDeviceManager &deviceManager);
    ~Project() = default;

    AudioDeviceManager &getDeviceManager() { return deviceManager; }
    InstrumentPlayer &getInstrumentPlayer() { return instrumentPlayer; }
    Synthesiser *getLiveSynth();

    const float getTempo() const { return tempo; };
    void setTempo(const float newTempo) { tempo = newTempo; }

    const TimeSignature &getTimeSignature() const { return timeSignature; }
    void setTimeSignature(const TimeSignature &newTimeSignature) { timeSignature = newTimeSignature; }

    Track *addTrack();
    void deleteTrack(Track *track);

    Sample *addSample(
        Track &track, const File &file, double startPos, double endPos, AudioFormatManager &formatManager);

    TrackList &getTrackList() { return trackList; }
    Mixer &getMixer() { return mixer; }
    Transport &getTransport() { return transport; }

    Track *getSelectedTrack() const { return trackList.getSelectedTrack(); }
    Sample *getSelectedSample() const { return trackList.getSelectedSample(); }

    bool isRecording() const;
    MidiRecorder *getMidiRecorder() { return trackList.getMidiRecorder(); }
    MidiKeyboardState &getKeyboardState() { return keyboardState; }

    int secondsToTicks(double seconds) const;
    double ticksToSeconds(int ticks) const;
    double measuresToSeconds(double measures) const;
    double secondsToMeasures(double seconds) const;

    double getVerticalScale() const { return verticalScale; }
    double getHorizontalScale() const { return horizontalScale; }
    //    double getHorizontalScaleRatio() const { return initialHorizontalScale / horizontalScale; }

    void incrementVerticalScale() { verticalScale *= 1.08; }
    void decrementVerticalScale() {
        verticalScale /= 1.08;
        verticalScale = max(verticalScale, 0.0000001);
    }
    void incrementHorizontalScale() { horizontalScale *= 1.2; }
    void decrementHorizontalScale() {
        horizontalScale /= 1.2;
        horizontalScale = max(horizontalScale, 0.0000001);
    }

    string to_json();
    void from_json(AudioFormatManager &formatManager, string filename);

    void writeAudioFile(const File &file);

    void printEvents(const MidiMessageSequence &midiMessages) const;
    void printEvents(const MidiBuffer &buffer) const;
    void printEvent(const MidiMessage &event, double sampleRate) const;

  private:
    const float initialTempo = 120.0;
    const double initialVerticalScale = 1;
    const double initialHorizontalScale = 100;
    const double scaleIncrement = 5;

    AudioDeviceManager &deviceManager;
    MidiKeyboardState keyboardState;
    InstrumentPlayer instrumentPlayer;
    TrackList trackList;
    Mixer mixer;
    Transport transport;
    TimeSignature timeSignature;
    float tempo = initialTempo;
    double verticalScale = initialVerticalScale;
    double horizontalScale = initialHorizontalScale;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Project)
};

} // namespace trackman
