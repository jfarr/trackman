#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/MidiRecorder.h"
#include "audio/PositionableMixingAudioSource.h"
#include "audio/SamplePlayer.h"
#include "audio/SynthAudioSource.h"

class TrackList;

class Track {
  public:
    Track(MidiRecorder &midiRecorder, juce::AudioDeviceManager &deviceManager);
    ~Track();

    int getTrackNumber() const { return trackNumber; }
    juce::String getName() const { return name; }
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSoloed() const { return soloed; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }
    juce::int64 getTotalLengthInSamples() const;
    juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }

    juce::PositionableAudioSource *getSource() { return meteredSource.get(); }
    foleys::LevelMeterSource *getMeterSource() {
        return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource();
    }

    void setTrackNumber(int newNumber) { trackNumber = newNumber; }
    void setName(const juce::String &newName) { name = newName; }
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
    const juce::MidiMessageSequence &getMidiMessages() const { return midiMessages; }
    const juce::MidiMessageSequence getCurrentMidiMessages(double pos) const;
    void setMidiMessages(const juce::MidiMessageSequence &newMessages) { midiMessages = newMessages; }

    void processNextMidiBuffer(
        juce::MidiBuffer &buffer, const int startSample, const int numSamples, const juce::int64 currentPos);

  private:
    friend TrackList;

    Sample *addSample(const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain(bool anySoloed);

    const juce::String defaultName = "untitled";

    int trackNumber = 0;
    juce::String name = defaultName;
    std::unique_ptr<MeteredAudioSource> meteredSource;
    std::unique_ptr<GainAudioSource> gainSource;

    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    bool recording = false;

    std::list<std::shared_ptr<Sample>> samples;
    std::unique_ptr<SamplePlayer> samplePlayer;

    juce::AudioDeviceManager &deviceManager;
    MidiRecorder &midiRecorder;
    SynthAudioSource synthAudioSource;
    juce::MidiMessageSequence midiMessages;

    void createSamplePlayer();
    void removeSamplePlayer();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
