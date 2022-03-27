#include "NoteRoll.h"
#include "Project.h"
#include "Track.h"
#include "common/midiutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, Track &track, double recordStartPosInSeconds) : project(project), track(track) {}

MidiMessageSequence NoteRoll::getMidiMessages() const { return midiMessages; }

double NoteRoll::getStartPosInSeconds() const {
    auto startTimeInTicks = midiMessages.getStartTime();
    auto firstNotePosInSeconds = project.ticksToSeconds(startTimeInTicks);
    return max(startPosInSeconds, firstNotePosInSeconds);
}

double NoteRoll::getEndPosInSeconds() const { return endPosInSeconds; }

double NoteRoll::getLengthInSeconds() const { return endPosInSeconds - startPosInSeconds; }

void NoteRoll::startRecording() { recording = true; }

void NoteRoll::stopRecording() { recording = false; }

MidiMessageSequence::MidiEventHolder *NoteRoll::addEvent(const MidiMessage &newMessage) {
    if (midiMessages.getNumEvents() == 0) {
        startPosInTicks = newMessage.getTimeStamp();
        startPosInSeconds = project.ticksToSeconds(startPosInTicks);
    }
    if (newMessage.isNoteOff()) {
        endPosInTicks = newMessage.getTimeStamp() + 1;
        endPosInSeconds = project.ticksToSeconds(endPosInTicks);
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
    DBG("start tick: " << startTimeInTicks << " end tick: " << endTimeInTicks);
    auto relativeStartTick = max(0, startTimeInTicks - startPosInTicks);
    auto relativeEndTick = endTimeInTicks - startPosInTicks;
    DBG("relativeStartTick: " << relativeStartTick << " relativeEndTick: " << relativeEndTick);
    auto startIndex = midiMessages.getNextIndexAtTime(relativeStartTick);
    auto endIndex = midiMessages.getNextIndexAtTime(relativeEndTick);
    DBG("startIndex: " << startIndex << " endIndex:" << endIndex);
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        DBG("NoteRoll::processNextMidiBuffer adding " << (event.isNoteOn() ? "note on" : "note off")
                                                      << " event at: " << event.getTimeStamp());
        buffer.addEvent(event, event.getTimeStamp());
    }
    DBG("playing notes:");
    project.printEvents(buffer);
}

void NoteRoll::printEvents() const { project.printEvents(midiMessages); }

} // namespace trackman
