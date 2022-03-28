#pragma once

#include <JuceHeader.h>

#include "MidiHandler.h"
#include "NoteRoll.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class MidiRecorder : public MidiHandler {
  public:
    MidiRecorder(NoteRoll &noteRoll, AudioDeviceManager &deviceManager);
    ~MidiRecorder() override;

    void startRecording();
    void stopRecording();
    bool isRecording() const;
    NoteRoll &getNoteRoll() { return noteRoll; }

    void handleMessage(MidiMessage message, double time);
    MidiMessageSequence getMidiMessages(double posInSeconds) const;

    void printEvents() const;

  private:
    NoteRoll &noteRoll;
    AudioDeviceManager &deviceManager;
    bool recording = false;
    bool looping = false;

    CriticalSection mutex;

    void dispatchMessage(MidiMessage message, double timestampInTicks) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};

} // namespace trackman
