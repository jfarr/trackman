#pragma once

#include <JuceHeader.h>

#include "NoteRoll.h"

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class MidiRecorder : public MidiKeyboardState::Listener, MidiInputCallback {
  public:
    MidiRecorder(NoteRoll &noteRoll, MidiKeyboardState &keyboardState, AudioDeviceManager &deviceManager);
    ~MidiRecorder() override;

    MidiKeyboardState &getKeyboardState() { return keyboardState; }

    void startRecording();
    void stopRecording();
    bool isRecording() const;
    NoteRoll &getNoteRoll() { return noteRoll; }

    MidiMessageSequence getMidiMessages(double pos) const;

    void setMidiInput(int index);

    void printEvents() const;

    function<void(const MidiMessage &message, double time)> onMidiMessage = nullptr;

    //==============================================================================
    // MidiKeyboardState::Listener
    void handleNoteOn(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;

    //==============================================================================
    // MidiInputCallback
    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) override;

  private:
    class MidiMessageCallback : public CallbackMessage {
      public:
        MidiMessageCallback(MidiRecorder *o, const MidiMessage &m, double t) : owner(o), message(m), time(t) {}

        void messageCallback() override { owner->handleMessage(message, time); }

        MidiRecorder *owner;
        MidiMessage message;
        double time;
    };
    void postMessage(const MidiMessage &message, double time);
    void handleMessage(MidiMessage message, double time);

    NoteRoll &noteRoll;
    AudioDeviceManager &deviceManager;
    MidiKeyboardState &keyboardState;
    bool recording = false;
    bool looping = false;
    bool isAddingFromMidiInput = false;
    int lastInputIndex = 0;

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};

} // namespace trackman
