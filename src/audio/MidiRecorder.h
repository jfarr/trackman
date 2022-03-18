#pragma once

#include "SynthAudioSource.h"
#include <JuceHeader.h>

class Project;

class MidiRecorder : public juce::PositionableAudioSource, public juce::MidiKeyboardState::Listener {
  public:
    MidiRecorder(Project &project, juce::AudioDeviceManager &deviceManager);
    ~MidiRecorder() override;

    juce::MidiKeyboardState &getKeyboardState() { return keyboardState; }
    const juce::MidiMessageSequence &getMidiMessages() const { return midiMessages; }
    bool isRecording() const;

    void setMidiMessages(const juce::MidiMessageSequence &newMessages) { midiMessages = newMessages; }
    void startRecording();
    void stopRecording();
    void reset();

    //==============================================================================
    // juce::MidiKeyboardState::Listener
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 position) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override { return false; }
    void setLooping(bool shouldLoop) override {}

    static void printEvents(const juce::MidiMessageSequence &midiMessages);

  private:
    class MidiMessageCallback : public juce::CallbackMessage {
      public:
        MidiMessageCallback(MidiRecorder *o, const juce::MidiMessage &m, double t) : owner(o), message(m), time(t) {}

        void messageCallback() override { owner->handleMessage(message, time); }

        MidiRecorder *owner;
        juce::MidiMessage message;
        double time;
    };
    void postMessage(juce::MidiMessage &message, double time);
    void handleMessage(juce::MidiMessage &message, double time);

    Project &project;
    juce::AudioDeviceManager &deviceManager;
    juce::MidiKeyboardState keyboardState;
    juce::MidiMessageSequence midiMessages;
    juce::int64 nextReadPosition = 0;
    juce::int64 lastSampleNumber = 0;
    bool recording = false;

    juce::CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};
