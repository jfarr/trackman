#include "NoteRoll.h"
#include "Project.h"
#include "common/midiutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project) : project(project) {}

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
    //    if (resamplingSource != nullptr) {
    //        resamplingSource->prepareToPlay(blockSize, sampleRate);
    //    }
}

void NoteRoll::releaseResources() {
    //    if (resamplingSource != nullptr) {
    //        resamplingSource->releaseResources();
    //    }
}

void NoteRoll::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    //    if (resamplingSource != nullptr) {
    //        resamplingSource->getNextAudioBlock(bufferToFill);
    //    }
}

//==============================================================================
void NoteRoll::setNextReadPosition(int64 newPosition) {
    //    if (resamplingSource != nullptr) {
    //        resamplingSource->setNextReadPosition(newPosition);
    //    }
}

int64 NoteRoll::getNextReadPosition() const {
    //    return resamplingSource == nullptr ? 0 : resamplingSource->getNextReadPosition();
    return 0;
}

int64 NoteRoll::getTotalLength() const {
    //    return resamplingSource == nullptr ? 0 : resamplingSource->getTotalLength();
    return 0;
}

bool NoteRoll::isLooping() const { return false; }

void NoteRoll::setLooping(bool shouldLoop) {
    //    if (resamplingSource != nullptr) {
    //        resamplingSource->setLooping(shouldLoop);
    //    }
}

} // namespace trackman
