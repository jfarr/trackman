#pragma once

#include "MidiListener.h"
#include "SynthAudioSource.h"
#include <JuceHeader.h>

class MidiRecorder : public juce::PositionableAudioSource, public juce::MidiKeyboardState::Listener {
  public:
    MidiRecorder(juce::AudioDeviceManager &deviceManager, juce::AudioTransportSource &transport);
    ~MidiRecorder() override;

    juce::MidiKeyboardState &getKeyboardState() { return keyboardState; }
    bool isRecording() const { return recording; }

    void setAudioSource(juce::AudioSource *newSource) { source = newSource; }
    void startRecording();
    void stopRecording();
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    //    juce::int64 getTotalLengthInSamples() { return nextReadPosition; }

    void processNextMidiBuffer(juce::MidiBuffer &buffer, double time, int startSample, int numSamples);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 position) override { nextReadPosition = position; }
    juce::int64 getNextReadPosition() const override { return nextReadPosition; }
    juce::int64 getTotalLength() const override { return recording ? nextReadPosition : lastSampleNumber; }
    bool isLooping() const override { return false; }
    void setLooping(bool shouldLoop) override {}

  private:
    juce::AudioDeviceManager &deviceManager;
    juce::AudioTransportSource &transport;
    juce::AudioSource *source;
    juce::MidiKeyboardState keyboardState;
    juce::Array<juce::MidiBuffer> midiEvents;
    juce::int64 nextReadPosition;
    juce::int64 lastSampleNumber = 0;
    bool recording = false;

    juce::MidiBuffer &getBufferAtSampleNumber(juce::int64 sampleNumber);
    juce::MidiBuffer &getBufferAtTime(double time);
    juce::int64 getSampleNumber(double time) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};
