#pragma once

#include <JuceHeader.h>

#include "MidiPlayer.h"
#include "MidiRecorder.h"
#include "NoteRoll.h"
#include "Sample.h"
#include "SamplePlayer.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;
class TrackList;

class Track {
  public:
    Track(Project &project, AudioDeviceManager &deviceManager);
    ~Track();

    int getTrackNumber() const { return trackNumber; }
    String getName() const { return name; }
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSoloed() const { return soloed; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }
    int64 getTotalLengthInSamples() const;
    AudioDeviceManager &getDeviceManager() { return deviceManager; }

    PositionableAudioSource *getSource() { return meteredSource.get(); }
    foleys::LevelMeterSource *getMeterSource() {
        return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource();
    }

    void setTrackNumber(int newNumber) { trackNumber = newNumber; }
    void setName(const String &newName) { name = newName; }
    void setLevelGain(float newLevel);
    void setSelected(bool newSelected);
    Sample *getSelected() const;
    void setDeleted(bool newDeleted);

    void selectSample(Sample *newSelected);
    void moveSampleTo(Sample &sample, Track &toTrack);
    void eachSample(function<void(Sample &sample)> f);
    bool hasSamples() const { return !samples.empty(); }
    int getNumSamples() const { return samples.size(); }

    bool hasMidi() const { return !noteRolls.empty(); }
    bool canRecord() const { return samplePlayer == nullptr; }
    bool isRecording() const { return midiRecorder != nullptr; }
    MidiRecorder *getMidiRecorder() { return midiRecorder.get(); }
    void startRecording();
    void pauseRecording();
    void stopRecording();

    void eachNoteRoll(function<void(NoteRoll &noteRoll)> f);
    const MidiMessageSequence &getMidiMessages() const { return noteRolls.back()->getMidiMessages(); }
    const MidiMessageSequence getCurrentMidiMessages(double pos) const;
    void processNextMidiBuffer(MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) const;
    double getMidiLengthInSeconds() const;
    int64 getMidiLengthInSamples() const;
//    void setMidiMessages(const MidiMessageSequence &newMessages);
    void updateCurrentNoteRoll();

  private:
    friend TrackList;

    Sample *addSample(const File &file, double startPosInSeconds, double endPosInSeconds, AudioFormatManager &formatManager);
    NoteRoll *addNoteRoll();
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain(bool anySoloed);

    const String defaultName = "untitled";

    int trackNumber = 0;
    String name = defaultName;
    unique_ptr<MeteredAudioSource> meteredSource;
    unique_ptr<GainAudioSource> gainSource;

    float level = Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
//    bool recording = false;
    double recordStartPosInSeconds = 0;

    list<shared_ptr<Sample>> samples;
    unique_ptr<SamplePlayer> samplePlayer;

    Project &project;
    AudioDeviceManager &deviceManager;
    unique_ptr<MidiRecorder> midiRecorder = nullptr;
    list<shared_ptr<NoteRoll>> noteRolls;
    MidiPlayer midiPlayer;

    void createSamplePlayer();
    void removeSamplePlayer();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace trackman
