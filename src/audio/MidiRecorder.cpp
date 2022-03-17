#include "MidiRecorder.h"

#include <limits>

MidiRecorder::MidiRecorder(juce::AudioTransportSource &transport) : transport(transport) {
    keyboardState.addListener(this);
}

MidiRecorder::~MidiRecorder() { keyboardState.removeListener(this); }

void MidiRecorder::startRecording(double newSampleRate) {
    sampleRate = newSampleRate;
}

void MidiRecorder::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto timestamp = transport.getCurrentPosition();
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(timestamp);
    auto buffer = getBufferAtTime(timestamp);
    buffer.addEvent(m, timestamp * sampleRate);
    DBG("added note on event at time: " << timestamp << " sample number: " << (timestamp * sampleRate));
}

void MidiRecorder::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto timestamp = transport.getCurrentPosition();
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    m.setTimeStamp(timestamp);
    auto buffer = getBufferAtTime(timestamp);
    buffer.addEvent(m, timestamp * sampleRate);
    DBG("added note off event at time: " << timestamp << " sample number: " << (timestamp * sampleRate));
}

juce::MidiBuffer &MidiRecorder::getBufferAtTime(double time) { return getBufferAtSampleNumber(getSampleNumber(time)); }

juce::MidiBuffer &MidiRecorder::getBufferAtSampleNumber(juce::int64 sampleNumber) {
    int index = sampleNumber / INT_MAX;
    DBG("accessing buffer " << index);
    if (index >= midiEvents.size()) {
        DBG("expanding buffer to size " << index + 1);
        midiEvents.resize(index + 1);
    }
    return midiEvents.getReference(index);
}

void MidiRecorder::processNextMidiBuffer(juce::MidiBuffer &buffer, double time, int startSample, int numSamples) {
    int sampleNumber = getSampleNumber(time) + startSample;
    int startIndex = sampleNumber / INT_MAX;
    int endIndex = (sampleNumber + numSamples) / INT_MAX;
    int j = startSample;
    for (int i = startIndex; i <= endIndex; i++) {
        auto &bufferToCopy = getBufferAtSampleNumber(i * INT_MAX);
        for (auto event : bufferToCopy) {
            if (event.samplePosition >= startSample && event.samplePosition < startSample + numSamples) {
                buffer.addEvent(event.getMessage(), j++);
            }
        }
    }
}

void MidiRecorder::prepareToPlay(int blockSize, double sampleRate) {
    if (source != nullptr) {
        source->prepareToPlay(blockSize, sampleRate);
    }
}

void MidiRecorder::releaseResources() {
    if (source != nullptr) {
        source->releaseResources();
    }
}

void MidiRecorder::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    nextReadPosition += bufferToFill.numSamples;
    if (source != nullptr) {
        source->getNextAudioBlock(bufferToFill);
    }
}
