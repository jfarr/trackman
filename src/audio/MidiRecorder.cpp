#include "MidiRecorder.h"
#include "model/Project.h"

MidiRecorder::MidiRecorder(Project &project, juce::AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager) {
    keyboardState.addListener(this);
}

MidiRecorder::~MidiRecorder() { keyboardState.removeListener(this); }

void MidiRecorder::startRecording() {
    const juce::ScopedLock lock(mutex);
    keyboardState.reset();
    recording = true;
}

void MidiRecorder::stopRecording() {
    const juce::ScopedLock lock(mutex);
    recording = false;
}

void MidiRecorder::reset() {
    keyboardState.reset();
}

bool MidiRecorder::isRecording() const {
    const juce::ScopedLock lock(mutex);
    return recording;
}

//==============================================================================
void MidiRecorder::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (!recording) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m, t);
}

void MidiRecorder::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (!recording) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m, t);
}

void MidiRecorder::postMessage(juce::MidiMessage &message, double time) {
    (new MidiMessageCallback(this, message, time))->post();
}

void MidiRecorder::handleMessage(juce::MidiMessage &message, double time) {
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto offset = (time - t) * .001;
    auto timestamp = project.getMixer().getTransportSource().getCurrentPosition();
    message.setTimeStamp(timestamp + offset);
    auto sampleNumber = timestamp * deviceManager.getAudioDeviceSetup().sampleRate;
    lastSampleNumber = sampleNumber;
    midiMessages.addEvent(message);
}

//==============================================================================
void MidiRecorder::setNextReadPosition(juce::int64 position) { nextReadPosition = position; }
juce::int64 MidiRecorder::getNextReadPosition() const { return nextReadPosition; }

juce::int64 MidiRecorder::getTotalLength() const {
    return recording ? std::max(nextReadPosition, lastSampleNumber) : lastSampleNumber;
}

//==============================================================================
void MidiRecorder::prepareToPlay(int blockSize, double sampleRate) {
}

void MidiRecorder::releaseResources() {
}

void MidiRecorder::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    nextReadPosition += bufferToFill.numSamples;
}

void MidiRecorder::printEvents(const juce::MidiMessageSequence &midiMessages) {
    for (auto i = midiMessages.begin(); i != midiMessages.end(); i++) {
        auto m = (*i)->message;
        DBG(juce::String("note ") + (m.isNoteOn() ? "on" : "off") + " event at time "
            << m.getTimeStamp() << ": noteNumber=" << m.getNoteNumber() << " velocity=" << m.getVelocity());
    }
}
