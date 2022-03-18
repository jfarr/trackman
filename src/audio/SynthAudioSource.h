#pragma once

#include <JuceHeader.h>

struct SineWaveSound : public juce::SynthesiserSound {
    SineWaveSound() = default;

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

struct SineWaveVoice : public juce::SynthesiserVoice {
    SineWaveVoice() = default;

    bool canPlaySound(juce::SynthesiserSound *sound) override {
        return dynamic_cast<SineWaveSound *>(sound) != nullptr;
    }

    void startNote(
        int midiNoteNumber, float velocity, juce::SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override {
        if (allowTailOff) {
            if (tailOff == 0.0)
                tailOff = 1.0;
        } else {
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override {
        if (angleDelta != 0.0) {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0) {
                    auto currentSample = (float)(std::sin(currentAngle) * level * tailOff);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;

                    tailOff *= 0.99; // [8]

                    if (tailOff <= 0.005) {
                        clearCurrentNote(); // [9]

                        angleDelta = 0.0;
                        break;
                    }
                }
            } else {
                while (--numSamples >= 0) // [6]
                {
                    auto currentSample = (float)(std::sin(currentAngle) * level);

                    for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample(i, startSample, currentSample);

                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }

  private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;
};

class SynthAudioSource : public juce::PositionableAudioSource {
  public:
    explicit SynthAudioSource(juce::MidiKeyboardState &keyState) : keyboardState(keyState) {
        for (auto i = 0; i < 4; ++i) // [1]
            synth.addVoice(new SineWaveVoice());

        synth.addSound(new SineWaveSound()); // [2]
    }

    void setUsingSineWaveSound() { synth.clearSounds(); }

    void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override {
        synth.setCurrentPlaybackSampleRate(sampleRate); // [3]
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        bufferToFill.clearActiveBufferRegion();

        juce::MidiBuffer incomingMidi;
        keyboardState.processNextMidiBuffer(
            incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true); // [4]

        synth.renderNextBlock(
            *bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples); // [5]
    }

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override {}
    juce::int64 getNextReadPosition() const override { return 0; }
    juce::int64 getTotalLength() const override { return 0; }
    bool isLooping() const override { return false; }
    void setLooping(bool shouldLoop) override {}

  private:
    juce::MidiKeyboardState &keyboardState;
    juce::Synthesiser synth;
};
