#include "NoteRoll.h"
#include "Project.h"
#include "Track.h"
#include "common/midiutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, Track &track, double recordStartPosInSeconds)
    : project(project), track(track), currentSampleRate(project.getDeviceManager().getAudioDeviceSetup().sampleRate) {}

MidiMessageSequence NoteRoll::getMidiMessages() const { return midiMessages; }

double NoteRoll::getStartPosInSeconds() const {
    auto startTimeInTicks = midiMessages.getStartTime();
    auto firstNotePosInSeconds = project.ticksToSeconds(startTimeInTicks);
    return max(startPosInSeconds, firstNotePosInSeconds);
}

double NoteRoll::getEndPosInSeconds() const { return endPosInSeconds; }

double NoteRoll::getLengthInSeconds() const { return endPosInSeconds - startPosInSeconds; }

// int64 NoteRoll::getTotalLengthInSamples() const { return getPositionFromTime(getEndPosInSeconds()); }

int64 NoteRoll::getPositionFromTime(double t) const { return t * currentSampleRate; }

void NoteRoll::startRecording() { recording = true; }

void NoteRoll::stopRecording() { recording = false; }

MidiMessageSequence::MidiEventHolder *NoteRoll::addEvent(const MidiMessage &newMessage) {
    if (midiMessages.getNumEvents() == 0) {
        startPosInSeconds = project.ticksToSeconds(newMessage.getTimeStamp());
    }
    if (newMessage.isNoteOff()) {
        endPosInSeconds = project.ticksToSeconds(newMessage.getTimeStamp() + 1);
    }
//    lengthInSeconds = max(0.0, endPosInSeconds - startPosInSeconds);
//    sourceLengthInSeconds = lengthInSeconds;
    //    auto offset = newMessage.getTimeStamp() - startPosInSeconds;
    auto offset = project.secondsToTicks(startPosInSeconds);
    auto event = midiMessages.addEvent(newMessage.withTimeStamp(newMessage.getTimeStamp() - offset));
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

//==============================================================================
void NoteRoll::prepareToPlay(int blockSize, double sampleRate) {
    const ScopedLock lock(mutex);
    currentSampleRate = sampleRate;
    //    track.getSynth().allNotesOff(0, false);
}

void NoteRoll::releaseResources() {}

void NoteRoll::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
//    MidiBuffer midiEvents;
    bufferToFill.clearActiveBufferRegion();

    if (looping) {
        auto sourceLengthInSamples = (int64)(getLengthInSeconds() * currentSampleRate);
        auto newStartPos = currentPosition % sourceLengthInSamples;
        auto newEndPos = (currentPosition + bufferToFill.numSamples) % sourceLengthInSamples;
        if (newEndPos < newStartPos) {
            DBG("WRAPPED!!!!!!");
            MidiBuffer midiEvents1;
            processMidiBuffer(midiEvents1, newStartPos, sourceLengthInSamples);
            MidiBuffer midiEvents2;
            processMidiBuffer(midiEvents2, 0, newEndPos);
        } else {
            MidiBuffer midiEvents;
            processMidiBuffer(midiEvents, newStartPos, newEndPos);
        }
    } else {
        MidiBuffer midiEvents;
        processMidiBuffer(midiEvents, currentPosition, currentPosition + bufferToFill.numSamples);
//        track.getInstrument().renderNextPlaybackBlock(
//            *bufferToFill.buffer, &midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
    }
    currentPosition += bufferToFill.numSamples;
//
//
//
//    int64 sourceLengthInSamples = (int64)getLengthInSeconds() * (int64)currentSampleRate;
//    //    DBG("endPosInSeconds: " << endPosInSeconds);
//    //    DBG("sourceLengthInSamples: " << sourceLengthInSamples);
//    auto pos = looping ? currentPosition % sourceLengthInSamples : currentPosition;
//    //    DBG("note roll pos: " << pos << " to " << pos + bufferToFill.numSamples);
//    processNextMidiBuffer(midiEvents, bufferToFill.startSample, bufferToFill.numSamples, pos);
//    //    DBG("NoteRoll::getNextAudioBlock writing to buffer: " << String::toHexString((long)bufferToFill.buffer));
//    //    track.getSynth().renderNextPlaybackBlock(
//    //        *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
//
//    //    instrument.renderNextPlaybackBlock(
//    //        *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
//
//    currentPosition += bufferToFill.numSamples;
}

void NoteRoll::processMidiBuffer(MidiBuffer &buffer, int64 startTimeInSamples, int64 endTimeInSamples) {
    const double startTimeInSeconds = startTimeInSamples / currentSampleRate;
    const double endTimeInSeconds = endTimeInSamples / currentSampleRate;
//    DBG("NoteRoll::processNextMidiBuffer looping: " << (looping ? "true" : "false")
//                                                    << " pos: " << pos << " secs: " << getLengthInSeconds() << " len: " << sourceLengthInSamples
//                                                    << " startPos: " << startPosInSeconds << " pos: " << pos << " ("
//                                                    << startTime << " to " << endTime << ")");

    //    auto midiMessages = getMidiMessages();
    auto startTick = project.secondsToTicks(startTimeInSeconds);
    auto endTick = project.secondsToTicks(endTimeInSeconds);
    DBG("start tick: " << startTick << " end tick: " << endTick);
    auto startIndex = midiMessages.getNextIndexAtTime(startTick);
    auto endIndex = midiMessages.getNextIndexAtTime(endTick);
    //    DBG("startIndex: " << startIndex << " endIndex:" << endIndex);
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        DBG("NoteRoll::processNextMidiBuffer adding " << (event.isNoteOn() ? "note on" : "note off")
                                                      << " event at: " << event.getTimeStamp());
        buffer.addEvent(event, event.getTimeStamp());
    }
}

void NoteRoll::processNextMidiBuffer(
    MidiBuffer &buffer, const int /*startSample*/, const int numSamples, const int64 currentPos) {
    const ScopedLock lock(mutex);
//    auto sourceLengthInSamples = (int64)(getLengthInSeconds() * (double)currentSampleRate);
    if (looping) {
        auto sourceLengthInSamples = (int64)(getLengthInSeconds() * currentSampleRate);
        auto newStartPos = currentPosition % sourceLengthInSamples;
        auto newEndPos = (currentPosition + numSamples) % sourceLengthInSamples;
        if (newEndPos < newStartPos) {
            DBG("WRAPPED!!!!!!");
            processMidiBuffer(buffer, newStartPos, sourceLengthInSamples);
            processMidiBuffer(buffer, 0, newEndPos);
        } else {
            processMidiBuffer(buffer, newStartPos, newEndPos);
        }
    } else {
        processMidiBuffer(buffer, currentPosition, currentPosition + numSamples);
    }
    currentPosition += numSamples;

//    auto pos = looping ? currentPosition % sourceLengthInSamples : currentPosition;
//
//    //    const int64 pos = currentPosition - (int64)startPosInSeconds * (int64)currentSampleRate;
//    const double startTime = pos / currentSampleRate;
//    const double endTime = startTime + numSamples / currentSampleRate;
//    DBG("NoteRoll::processNextMidiBuffer looping: " << (looping ? "true" : "false")
//                                                    << " pos: " << pos << " secs: " << getLengthInSeconds() << " len: " << sourceLengthInSamples
//                                                    << " startPos: " << startPosInSeconds << " pos: " << pos << " ("
//                                                    << startTime << " to " << endTime << ")");
//
//    //    auto midiMessages = getMidiMessages();
//    auto startTick = project.secondsToTicks(startTime);
//    auto endTick = project.secondsToTicks(endTime);
//    DBG("start tick: " << startTick << " end tick: " << endTick);
//    auto startIndex = midiMessages.getNextIndexAtTime(startTick);
//    auto endIndex = midiMessages.getNextIndexAtTime(endTick);
//    //    DBG("startIndex: " << startIndex << " endIndex:" << endIndex);
//    for (int i = startIndex; i < endIndex; i++) {
//        auto p = midiMessages.getEventPointer(i);
//        auto event = p->message;
//        DBG("NoteRoll::processNextMidiBuffer adding " << (event.isNoteOn() ? "note on" : "note off")
//                                                      << " event at: " << event.getTimeStamp());
//        buffer.addEvent(event, event.getTimeStamp());
//    }
//    //    DBG("playing notes:");
//    //    printEvents(midiMessages);
//    currentPosition += numSamples;
}

//==============================================================================
void NoteRoll::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    currentPosition = newPosition;
}

int64 NoteRoll::getNextReadPosition() const {
    const ScopedLock lock(mutex);
    return currentPosition;
}

int64 NoteRoll::getTotalLength() const {
    auto pos = getEndPosInSeconds();
    auto len = getPositionFromTime(pos);
    //    len = len == 0 ? 0 : len + 102400;
    //    DBG("end pos: " << pos << " len: " << len);
    return len;
    //    return getPositionFromTime(getEndPosInSeconds()) + 1;
}

bool NoteRoll::isLooping() const { return looping; }

void NoteRoll::setLooping(bool shouldLoop) { looping = shouldLoop; }

void NoteRoll::printEvents() const { project.printEvents(midiMessages); }

} // namespace trackman
