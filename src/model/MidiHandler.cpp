#include "MidiHandler.h"
#include "Project.h"

namespace trackman {

MidiHandler::MidiHandler(Project &project) : project(project) {

    auto midiInputs = MidiInput::getAvailableDevices();
    StringArray midiInputNames;
    for (auto input : midiInputs) {
        midiInputNames.add(input.name);
    }
    // find the first enabled device and use that by default
    for (auto input : midiInputs) {
        if (project.getDeviceManager().isMidiInputDeviceEnabled(input.identifier)) {
            setMidiInput(midiInputs.indexOf(input));
            break;
        }
    }
    // if no enabled devices were found just use the first one in the list
    if (lastInputIndex == 0) {
        setMidiInput(0);
    }

    project.getKeyboardState().addListener(this);
}

MidiHandler::~MidiHandler() { project.getKeyboardState().removeListener(this); }

//==============================================================================
void MidiHandler::handleNoteOn(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = Time::getMillisecondCounterHiRes();
    auto m = MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    postMessage(m, t);
}

void MidiHandler::handleNoteOff(MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    if (isAddingFromMidiInput) {
        return;
    }
    auto t = Time::getMillisecondCounterHiRes();
    auto m = MidiMessage::noteOff(midiChannel, midiNoteNumber);
    postMessage(m, t);
}

void MidiHandler::postMessage(const MidiMessage &message, double time) {
    (new MidiMessageCallback(this, message, time))->post();
}

void MidiHandler::handleMessage(MidiMessage message, double time) {
    auto t = Time::getMillisecondCounterHiRes();
    auto offset = (time - t) * .001;
    auto &transport = project.getTransport().getTransportSource();
    double timestampInTicks = 0;
    if (transport.isPlaying()) {
        auto pos = project.getTransport().getTransportSource().getCurrentPosition();
        timestampInTicks = project.secondsToTicks(pos + offset);
        message.setTimeStamp(timestampInTicks);
    }
    if (onMidiMessage != nullptr) {
        onMidiMessage(message, timestampInTicks);
    }
}

void MidiHandler::setMidiInput(int index) {
    auto list = MidiInput::getAvailableDevices();
    auto &deviceManager = project.getDeviceManager();
    deviceManager.removeMidiInputDeviceCallback(list[lastInputIndex].identifier, this);
    auto newInput = list[index];
    if (!deviceManager.isMidiInputDeviceEnabled(newInput.identifier)) {
        deviceManager.setMidiInputDeviceEnabled(newInput.identifier, true);
    }
    deviceManager.addMidiInputDeviceCallback(newInput.identifier, this);
    lastInputIndex = index;
}

void MidiHandler::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message) {
    auto t = Time::getMillisecondCounterHiRes();
    const ScopedValueSetter<bool> scopedInputFlag(isAddingFromMidiInput, true);
    project.getKeyboardState().processNextMidiEvent(message);
    postMessage(message, t);
}

} // namespace trackman
