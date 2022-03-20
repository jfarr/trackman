#include "MidiRecorder.h"
#include "model/Project.h"

MidiRecorder::MidiRecorder(Project &project, juce::AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager) {

    auto midiInputs = juce::MidiInput::getAvailableDevices();
    juce::StringArray midiInputNames;
    for (auto input : midiInputs) {
        midiInputNames.add(input.name);
    }
    // find the first enabled device and use that by default
    for (auto input : midiInputs) {
        if (deviceManager.isMidiInputDeviceEnabled(input.identifier)) {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if (lastInputIndex == 0) {
        setMidiInput(0);
    }

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

bool MidiRecorder::isRecording() const {
    const juce::ScopedLock lock(mutex);
    return recording;
}

//==============================================================================
void MidiRecorder::handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m, t);
}

void MidiRecorder::handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m, t);
}

void MidiRecorder::postMessage(const juce::MidiMessage &message, double time) {
    (new MidiMessageCallback(this, message, time))->post();
}

void MidiRecorder::handleMessage(juce::MidiMessage message, double time) {
    auto t = juce::Time::getMillisecondCounterHiRes();
    auto offset = (time - t) * .001;
    auto timestamp = project.getMixer().getTransportSource().getCurrentPosition();
    message.setTimeStamp(project.secondsToTicks(timestamp + offset));
    midiMessages.addEvent(message);
}

void MidiRecorder::setMidiInput(int index) {
    auto list = juce::MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);
    auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier)) {
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
    }
    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    lastInputIndex = index;
}

void MidiRecorder::handleIncomingMidiMessage(juce::MidiInput *source, const juce::MidiMessage &message) {
    auto t = juce::Time::getMillisecondCounterHiRes();
    const juce::ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
    postMessage(message, t);
}

void MidiRecorder::printEvents(const juce::MidiMessageSequence &midiMessages) {
    for (auto i = midiMessages.begin(); i != midiMessages.end(); i++) {
        auto m = (*i)->message;
        DBG(juce::String("note ") + (m.isNoteOn() ? "on" : "off") + " event at time "
            << m.getTimeStamp() << ": noteNumber=" << m.getNoteNumber() << " velocity=" << m.getVelocity());
    }
}
