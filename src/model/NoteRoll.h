#pragma once

#include <JuceHeader.h>

#include "TrackRegion.h"
#include "audio/SynthAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class NoteRoll : public PositionableAudioSource, public TrackRegion {
  public:
    NoteRoll(Project &project);
    ~NoteRoll() = default;

    Project &getProject() { return project; }
    MidiMessageSequence getMidiMessages() const;
    bool empty() { return midiMessages.getNumEvents() == 0; }
    double getStartPosInSeconds() const;
    double getEndPosInSeconds() const;
    double getLengthInSeconds() const;
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }
    bool isRecording() const { return recording; }

    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

    void startRecording() { recording = true; }
    void stopRecording() { recording = false; }

    MidiMessageSequence::MidiEventHolder *addEvent(const MidiMessage &newMessage);
    void eachMidiMessage(function<void(const MidiMessageSequence::MidiEventHolder &eventHandle)> f) const;
    int getLowestNote() const;
    int getHighestNote() const;
    double getEndTime() const { return midiMessages.getEndTime(); }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    Project &project;
    MidiMessageSequence midiMessages;
    bool selected = false;
    bool deleted = false;
    bool recording = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

} // namespace trackman
