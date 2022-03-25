#include "NoteRoll.h"
#include "Project.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, const MidiMessageSequence &midiMessages, double startPosInSeconds, double endPosInSeconds)
    : project(project), midiMessages(midiMessages), startPosInSeconds(startPosInSeconds), endPosInSeconds(endPosInSeconds),
      lengthInSeconds(endPosInSeconds - startPosInSeconds) {}

void NoteRoll::setMidiMessages(const MidiMessageSequence &newMidiMessages) {
    midiMessages = newMidiMessages;
    update();
}

void NoteRoll::update() {
    midiMessages.sort();
    midiMessages.updateMatchedPairs();
    startPosInSeconds = project.ticksToSeconds(midiMessages.getStartTime());
    endPosInSeconds = project.ticksToSeconds(midiMessages.getEndTime());
    lengthInSeconds = endPosInSeconds - startPosInSeconds;
}

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
