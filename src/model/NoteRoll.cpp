#include "NoteRoll.h"
#include "Project.h"
#include "Track.h"
#include "common/midiutil.h"
#include "common/timeutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, Track &track) : project(project), track(track) {}

NoteRoll::NoteRoll(Project &project, Track &track, const int initialStartPosInTicks, const int initialEndPosInTicks,
    const MidiMessageSequence &initialMidiMessages)
    : NoteRoll(project, track) {
    midiMessages = initialMidiMessages;
    midiMessages.sort();
    midiMessages.updateMatchedPairs();
    startPosInTicks = initialStartPosInTicks;
    endPosInTicks = initialEndPosInTicks;
}

double NoteRoll::getStartPosInSeconds() const {
//    auto startTimeInTicks = midiMessages.getStartTime();
//    auto firstNotePosInSeconds = project.ticksToSeconds(startTimeInTicks);
//    return max(project.ticksToSeconds(startPosInTicks), firstNotePosInSeconds);
    return project.ticksToSeconds(startPosInTicks);
}

double NoteRoll::getEndPosInSeconds() const { return project.ticksToSeconds(endPosInTicks); }

double NoteRoll::getLengthInSeconds() const { return getEndPosInSeconds() - getStartPosInSeconds(); }

void NoteRoll::startRecording() { recording = true; }

void NoteRoll::stopRecording() { recording = false; }

MidiMessageSequence::MidiEventHolder *NoteRoll::addEvent(const MidiMessage &newMessage) {
    if (midiMessages.getNumEvents() == 0) {
        startPosInTicks = project.measureStartTick(newMessage.getTimeStamp());
    }
    if (newMessage.isNoteOff()) {
        endPosInTicks = project.measureEndTick(newMessage.getTimeStamp());
    }
    auto event = midiMessages.addEvent(newMessage.withTimeStamp(newMessage.getTimeStamp() - startPosInTicks));
    midiMessages.sort();
    midiMessages.updateMatchedPairs();
    return event;
}

void NoteRoll::eachMidiMessage(function<void(const MidiMessageSequence::MidiEventHolder &eventHandle)> f) const {
    for (auto m : midiMessages) {
        f(*m);
    }
}

int NoteRoll::getLowestNote() const { return midiutil::getLowestNote(midiMessages); }

int NoteRoll::getHighestNote() const { return midiutil::getHighestNote(midiMessages); }

void NoteRoll::processNextMidiBuffer(MidiBuffer &buffer, int startTimeInTicks, int endTimeInTicks) {
    auto relativeStartTick = max(0, startTimeInTicks - startPosInTicks);
    auto relativeEndTick = endTimeInTicks - startPosInTicks;
    auto startIndex = midiMessages.getNextIndexAtTime(relativeStartTick);
    auto endIndex = midiMessages.getNextIndexAtTime(relativeEndTick);
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        buffer.addEvent(event, event.getTimeStamp());
    }
}

string NoteRoll::toMidiFile() const {
    MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(timeutil::ticksPerQuarterNote);
    midiFile.addTrack(midiMessages);
    MemoryOutputStream out;
    midiFile.writeTo(out, 1);
    auto mb = out.getMemoryBlock();
    return Base64::toBase64(mb.getData(), mb.getSize()).toStdString();
}

void NoteRoll::printEvents() const { project.printEvents(midiMessages); }

} // namespace trackman
