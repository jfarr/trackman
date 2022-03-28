#include "MidiRecorder.h"
#include "model/Project.h"

namespace trackman {

MidiRecorder::MidiRecorder(NoteRoll &noteRoll, AudioDeviceManager &deviceManager)
    : MidiHandler(noteRoll.getProject()), noteRoll(noteRoll), deviceManager(deviceManager) {
}

MidiRecorder::~MidiRecorder() {}

void MidiRecorder::startRecording() {
    const ScopedLock lock(mutex);
    noteRoll.getProject().getKeyboardState().reset();
    recording = true;
    noteRoll.startRecording();
}

void MidiRecorder::stopRecording() {
    const ScopedLock lock(mutex);
    recording = false;
    noteRoll.stopRecording();
}

bool MidiRecorder::isRecording() const {
    const ScopedLock lock(mutex);
    return recording;
}

void MidiRecorder::dispatchMessage(MidiMessage message, double timestampInTicks) {
    noteRoll.addEvent(message);
    MidiHandler::dispatchMessage(message, timestampInTicks);
}

MidiMessageSequence MidiRecorder::getMidiMessages(double posInSeconds) const {
    MidiMessageSequence messages;
    noteRoll.eachMidiMessage([this, posInSeconds, &messages](const MidiMessageSequence::MidiEventHolder &eventHandle) {
        messages.addEvent(eventHandle.message);
        if (eventHandle.message.isNoteOn() && eventHandle.noteOffObject == nullptr) {
            auto noteOff = MidiMessage::noteOff(eventHandle.message.getChannel(), eventHandle.message.getNoteNumber());
            noteOff.setTimeStamp(noteRoll.getProject().secondsToTicks(posInSeconds - noteRoll.getStartPosInSeconds()));
            messages.addEvent(noteOff);
        }
    });
    messages.updateMatchedPairs();
    return messages;
}

void MidiRecorder::printEvents() const { noteRoll.printEvents(); }

} // namespace trackman
