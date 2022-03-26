#include "NoteRoll.h"
#include "Project.h"
#include "Track.h"
#include "common/midiutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, Track &track)
    : project(project), track(track), currentSampleRate(project.getDeviceManager().getAudioDeviceSetup().sampleRate) {}

MidiMessageSequence NoteRoll::getMidiMessages() const { return midiMessages; }

double NoteRoll::getStartPosInSeconds() const { return project.ticksToSeconds(midiMessages.getStartTime()); }

double NoteRoll::getEndPosInSeconds() const { return project.ticksToSeconds(midiMessages.getEndTime()); }

double NoteRoll::getLengthInSeconds() const { return getEndPosInSeconds() - getStartPosInSeconds(); }

MidiMessageSequence::MidiEventHolder *NoteRoll::addEvent(const MidiMessage &newMessage) {
    auto event = midiMessages.addEvent(newMessage);
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
}

void NoteRoll::releaseResources() {}

void NoteRoll::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
    MidiBuffer incomingMidi;
    bufferToFill.clearActiveBufferRegion();
    auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
    processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, pos);
    track.getSynth().renderNextBlock(
        *bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    currentPosition += bufferToFill.numSamples;
}

void NoteRoll::processNextMidiBuffer(
    MidiBuffer &buffer, const int /*startSample*/, const int numSamples, const int64 currentPos) const {
    const double startTime = currentPos / currentSampleRate;
    const double endTime = startTime + numSamples / currentSampleRate;

    auto midiMessages = getMidiMessages();
    auto startIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(startTime));
    auto endIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(endTime));
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        buffer.addEvent(event, event.getTimeStamp());
    }
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

int64 NoteRoll::getTotalLength() const { return 0; }

bool NoteRoll::isLooping() const { return looping; }

void NoteRoll::setLooping(bool shouldLoop) { looping = shouldLoop; }

} // namespace trackman
