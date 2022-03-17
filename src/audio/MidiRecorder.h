#pragma once

#include "MidiListener.h"
#include "SynthAudioSource.h"
#include <JuceHeader.h>

class MidiRecorder : public juce::MidiKeyboardState::Listener {
  public:
    MidiRecorder();
    ~MidiRecorder();

    juce::MidiKeyboardState &getKeyboardState() { return keyboardState; }

    void setSampleRate(double newSampleRate) { sampleRate = newSampleRate; }
    void startRecording(double newSampleRate);
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    juce::int64 getTotalLengthInSamples() { return getCurrentTimeInSeconds() * sampleRate; }

    void processNextMidiBuffer(juce::MidiBuffer &buffer, double time, int startSample, int numSamples);

  private:
    juce::MidiKeyboardState keyboardState;
    juce::Array<juce::MidiBuffer> midiEvents;
    double sampleRate = 0;
    double startTimeInSeconds = 0;

    double getCurrentTimeInSeconds() const;
    juce::MidiBuffer &getBufferAtSampleNumber(juce::int64 sampleNumber);
    juce::MidiBuffer &getBufferAtTime(double time);
    juce::int64 getSampleNumber(double time) const { return (juce::int64)time * (juce::int64)sampleRate; }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiRecorder)
};
