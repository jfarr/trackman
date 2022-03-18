#pragma once

#include "MidiListener.h"
#include "SynthAudioSource.h"
#include <JuceHeader.h>

class MidiRecorder : public juce::PositionableAudioSource, public juce::MidiKeyboardState::Listener {
  public:
    MidiRecorder(juce::AudioDeviceManager &deviceManager, juce::AudioTransportSource &transport);
    ~MidiRecorder() override;

    juce::MidiKeyboardState &getKeyboardState() { return keyboardState; }
    bool isRecording() const;

    void setAudioSource(juce::AudioSource *newSource) { source = newSource; }
    void startRecording();
    void stopRecording();

//    void processNextMidiBuffer(juce::MidiBuffer &buffer, double time, int startSample, int numSamples);

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

  private:
    class MidiMessageCallback : public juce::CallbackMessage {
      public:
        MidiMessageCallback(MidiRecorder *o, const juce::MidiMessage &m, double t)
            : owner(o), message(m), time(t) {}

        void messageCallback() override { owner->handleMessage(message, time); }

        MidiRecorder *owner;
        juce::MidiMessage message;
        double time;
    };
    void postMessage(juce::MidiMessage &message, double time);
    void handleMessage(juce::MidiMessage &message, double time);

    juce::AudioDeviceManager &deviceManager;
    juce::AudioTransportSource &transport;
    juce::AudioSource *source;
    juce::MidiKeyboardState keyboardState;
//    juce::Array<juce::MidiBuffer> midiEvents;
    juce::MidiMessageSequence midiEvents;
    juce::int64 nextReadPosition;
    juce::int64 lastSampleNumber = 0;
    bool recording = false;

//    juce::MidiBuffer &getBufferAtSampleNumber(juce::int64 sampleNumber);
//    juce::MidiBuffer &getBufferAtTime(double time);
    juce::int64 getSampleNumber(double time) const;

    void printEvents();

    juce::CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};
