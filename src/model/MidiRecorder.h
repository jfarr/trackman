#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class MidiRecorder : public MidiKeyboardState::Listener, MidiInputCallback {
  public:
    MidiRecorder(Project &project, AudioDeviceManager &deviceManager);
    ~MidiRecorder() override;

    MidiKeyboardState &getKeyboardState() { return keyboardState; }
    MidiMessageSequence &getMidiMessages() { return midiMessages; }
    bool isRecording() const;

    void setMidiMessages(MidiMessageSequence &newMessages) { midiMessages = newMessages; }
    void startRecording();
    void stopRecording();

    void setMidiInput(int index);

    static void printEvents(const MidiMessageSequence &midiMessages);

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

    Project &project;
    AudioDeviceManager &deviceManager;
    MidiKeyboardState keyboardState;
    MidiMessageSequence &midiMessages;
    MidiMessageSequence emptyMidiMessages;
    bool recording = false;
    bool looping = false;
    bool isAddingFromMidiInput = false;
    int lastInputIndex = 0;

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};

} // namespace trackman
