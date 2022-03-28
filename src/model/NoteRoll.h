#pragma once

#include <JuceHeader.h>

#include "Instrument.h"
#include "TrackRegion.h"
#include "audio/SynthAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;
class Track;

class NoteRoll : public TrackRegion {
  public:
    NoteRoll(Project &project, Track &track);
    NoteRoll(Project &project, Track &track, int startPosInTicks, int endPosInTicks,
        const MidiMessageSequence &midiMessages);
    ~NoteRoll() = default;

    Project &getProject() { return project; }
    bool empty() { return midiMessages.getNumEvents() == 0; }
    int getStartPosInTicks() const { return startPosInTicks; }
    int getEndPosInTicks() const { return endPosInTicks; }
    double getStartPosInSeconds() const override;
    double getEndPosInSeconds() const override;
    double getLengthInSeconds() const;
    bool isRecording() const { return recording; }

    void setPosition(double newPosInSeconds);

    void startRecording();
    void stopRecording();

    MidiMessageSequence::MidiEventHolder *addEvent(const MidiMessage &newMessage);
    void processNextMidiBuffer(MidiBuffer &buffer, int startTimeInTicks, int endTimeInTicks);

    void eachMidiMessage(function<void(const MidiMessageSequence::MidiEventHolder &eventHandle)> f) const;
    int getLowestNote() const;
    int getHighestNote() const;
    double getEndTime() const { return midiMessages.getEndTime(); }

    string toMidiFile() const;
    void printEvents() const;

  private:
    Project &project;
    Track &track;
    MidiMessageSequence midiMessages;
    int startPosInTicks = 0;
    int endPosInTicks = 0;
    bool recording = false;

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

} // namespace trackman
