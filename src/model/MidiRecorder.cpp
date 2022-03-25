#include "MidiRecorder.h"
#include "model/Project.h"

namespace trackman {

MidiRecorder::MidiRecorder(NoteRoll &noteRoll, MidiKeyboardState &keyboardState, AudioDeviceManager &deviceManager)
    : noteRoll(noteRoll), keyboardState(keyboardState), deviceManager(deviceManager) {

    auto midiInputs = MidiInput::getAvailableDevices();
    StringArray midiInputNames;
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
    const ScopedLock lock(mutex);
    keyboardState.reset();
    recording = true;
}

void MidiRecorder::stopRecording() {
    const ScopedLock lock(mutex);
    recording = false;
}

bool MidiRecorder::isRecording() const {
    const ScopedLock lock(mutex);
    return recording;
}

//==============================================================================
void MidiRecorder::handleNoteOn(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = Time::getMillisecondCounterHiRes();
    auto m = MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m, t);
}

void MidiRecorder::handleNoteOff(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = Time::getMillisecondCounterHiRes();
    auto m = MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m, t);
}

void MidiRecorder::postMessage(const MidiMessage &message, double time) {
    (new MidiMessageCallback(this, message, time))->post();
}

void MidiRecorder::handleMessage(MidiMessage message, double time) {
    auto t = Time::getMillisecondCounterHiRes();
    auto offset = (time - t) * .001;
    auto &project = noteRoll.getProject();
    auto &midiMessages = noteRoll.getMidiMessages();
    auto timestamp = project.getTransport().getTransportSource().getCurrentPosition();
    message.setTimeStamp(project.secondsToTicks(timestamp + offset));
    midiMessages.addEvent(message);
    if (onMidiMessage != nullptr) {
        onMidiMessage(message, time);
    }
}

void MidiRecorder::setMidiInput(int index) {
    auto list = MidiInput::getAvailableDevices();
    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);
    auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier)) {
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
    }
    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    lastInputIndex = index;
}

void MidiRecorder::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) {
    auto t = Time::getMillisecondCounterHiRes();
    const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    keyboardState.processNextMidiEvent(message);
    postMessage(message, t);
}

void MidiRecorder::printEvents(const MidiMessageSequence &midiMessages) {
    for (auto i = midiMessages.begin(); i != midiMessages.end(); i++) {
        auto m = (*i)->message;
        DBG(String("note ") + (m.isNoteOn() ? "on" : "off") + " event at time "
            << m.getTimeStamp() << ": noteNumber=" << m.getNoteNumber() << " velocity=" << m.getVelocity());
    }
}

} // namespace trackman
