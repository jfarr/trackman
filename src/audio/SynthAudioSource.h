#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

struct SineWaveSound : public SynthesiserSound {
    SineWaveSound() = default;

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

struct SineWaveVoice : public SynthesiserVoice {
    SineWaveVoice() = default;

    bool canPlaySound(SynthesiserSound *sound) override { return dynamic_cast<SineWaveSound *>(sound) != nullptr; }

    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *, int /*currentPitchWheelPosition*/) override {
//        DBG("startNote: " << midiNoteNumber);
        currentAngle = 0.0;
        level = velocity * 0.15;
        tailOff = 0.0;

        auto cyclesPerSecond = MidiMessage::getMidiNoteInHertz(midiNoteNumber);
        auto cyclesPerSample = cyclesPerSecond / getSampleRate();

        angleDelta = cyclesPerSample * 2.0 * MathConstants<double>::pi;
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override {
//        DBG("stopNote");
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

    void renderNextBlock(AudioSampleBuffer &outputBuffer, int startSample, int numSamples) override {
        if (angleDelta != 0.0) {
            if (tailOff > 0.0) // [7]
            {
                while (--numSamples >= 0) {
                    auto currentSample = (float)(sin(currentAngle) * level * tailOff);

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
                    auto currentSample = (float)(sin(currentAngle) * level);

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

} // namespace trackman
