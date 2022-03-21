#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/MidiRecorder.h"
#include "audio/PositionableMixingAudioSource.h"
#include "audio/SamplePlayer.h"
#include "audio/SynthAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;
class TrackList;

class Track {
  public:
    Track(Project &project, MidiRecorder &midiRecorder, AudioDeviceManager &deviceManager);
    ~Track();

    int getTrackNumber() const { return trackNumber; }
    String getName() const { return name; }
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSoloed() const { return soloed; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }
    int64 getTotalLengthInSamples() const;
    int64 getMidiLengthInSamples() const;
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
    void eachSample(std::function<void(Sample &sample)> f);
    bool hasSamples() const { return !samples.empty(); }
    int getNumSamples() const { return samples.size(); }

    bool hasMidi() const { return midiMessages.getNumEvents() > 0; }
    bool canRecord() const { return samplePlayer == nullptr; }
    bool isRecording() const { return recording; }
    void startRecording();
    void stopRecording();
    const MidiMessageSequence &getMidiMessages() const { return midiMessages; }
    const MidiMessageSequence getCurrentMidiMessages(double pos) const;
    void setMidiMessages(const MidiMessageSequence &newMessages);

    void processNextMidiBuffer(MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos);

  private:
    friend TrackList;

    Sample *addSample(const File &file, double startPos, double endPos, AudioFormatManager &formatManager);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain(bool anySoloed);

    const String defaultName = "untitled";

    int trackNumber = 0;
    String name = defaultName;
    std::unique_ptr<MeteredAudioSource> meteredSource;
    std::unique_ptr<GainAudioSource> gainSource;

    float level = Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    bool recording = false;

    std::list<std::shared_ptr<Sample>> samples;
    std::unique_ptr<SamplePlayer> samplePlayer;

    Project &project;
    AudioDeviceManager &deviceManager;
    MidiRecorder &midiRecorder;
    SynthAudioSource synthAudioSource;
    MidiMessageSequence midiMessages;

    void createSamplePlayer();
    void removeSamplePlayer();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};

} // namespace trackman
