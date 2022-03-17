#include "MidiRecorder.h"

MidiRecorder::MidiRecorder(juce::AudioDeviceManager &deviceManager, juce::AudioTransportSource &transport)
    : deviceManager(deviceManager), transport(transport) {
    keyboardState.addListener(this);
}

MidiRecorder::~MidiRecorder() { keyboardState.removeListener(this); }

void MidiRecorder::startRecording() {
    const juce::ScopedLock lock(mutex);
    recording = true;
}

void MidiRecorder::stopRecording() {
    const juce::ScopedLock lock(mutex);
    recording = false;
}

bool MidiRecorder::isRecording() const {
    const juce::ScopedLock lock(mutex);
    return recording;
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

juce::int64 MidiRecorder::getSampleNumber(double time) const {
    return (juce::int64)time * (juce::int64)deviceManager.getAudioDeviceSetup().sampleRate;
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

//==============================================================================
void MidiRecorder::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m);
}

void MidiRecorder::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m);
}

void MidiRecorder::postMessage(const juce::MidiMessage &message) { (new MidiMessageCallback(this, message))->post(); }

void MidiRecorder::handleMessage(const juce::MidiMessage &message) {
    auto timestamp = transport.getCurrentPosition();
    auto buffer = getBufferAtTime(timestamp);
    auto sampleNumber = timestamp * deviceManager.getAudioDeviceSetup().sampleRate;
    buffer.addEvent(message, sampleNumber);
    lastSampleNumber = sampleNumber;
    DBG("added note on event at time: " << timestamp << " sample number: " << sampleNumber);
}

//==============================================================================
void MidiRecorder::setNextReadPosition(juce::int64 position) {
    nextReadPosition = position;
}
juce::int64 MidiRecorder::getNextReadPosition() const {
    return nextReadPosition;
}

juce::int64 MidiRecorder::getTotalLength() const {
    return recording ? std::max(nextReadPosition, lastSampleNumber) : lastSampleNumber;
}

//==============================================================================
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
