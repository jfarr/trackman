#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class Project;

class MidiHandler : public MidiKeyboardState::Listener, public MidiInputCallback {
  public:
    MidiHandler(Project &project);
    ~MidiHandler() override;

//    MidiKeyboardState &getKeyboardState() { return keyboardState; }

//    MidiMessageSequence getMidiMessages(double posInSeconds) const;

    void setMidiInput(int index);

//    void printEvents() const;

    function<void(const MidiMessage &message, const double timeInTicks)> onMidiMessage = nullptr;

    //==============================================================================
    // MidiKeyboardState::Listener
    void handleNoteOn(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;

    //==============================================================================
    // MidiInputCallback
    void handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) override;

  protected:
    class MidiMessageCallback : public CallbackMessage {
      public:
        MidiMessageCallback(MidiHandler *o, const MidiMessage &m, double t) : owner(o), message(m), time(t) {}

        void messageCallback() override { owner->handleMessage(message, time); }

        MidiHandler *owner;
        MidiMessage message;
        double time;
    };
    void postMessage(const MidiMessage &message, double time);
    void handleMessage(MidiMessage message, double time);

    Project &project;
//    AudioDeviceManager &deviceManager;
//    MidiKeyboardState &keyboardState;
    MidiMessageCollector midiCollector;
    bool isAddingFromMidiInput = false;
    int lastInputIndex = 0;

  private:
    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiHandler)
};

} // namespace trackman
