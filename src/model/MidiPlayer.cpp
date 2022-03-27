#include "MidiPlayer.h"
#include "Project.h"
#include "Track.h"

namespace trackman {

MidiPlayer::MidiPlayer(Track &track) : track(track), noteRolls(track.getNoteRolls()) {}

void MidiPlayer::prepareToPlay(int blockSize, double sampleRate) {
    {
        const ScopedLock lock(mutex);
        currentSampleRate = sampleRate;
    }
}

void MidiPlayer::releaseResources() {
    const ScopedLock lock(mutex);
    currentSampleRate = 0;
}

void MidiPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
    auto currentEndPosition = currentPosition + bufferToFill.numSamples;
    if (bufferToFill.numSamples > 0) {
        Timeline timeline = getCurrentTimeline();
        auto sourceLengthInSamples = getTotalLength();
        auto startPositionInSamples = looping ? currentPosition % sourceLengthInSamples : currentPosition;
        auto endPositionInSamples = looping ? currentEndPosition % sourceLengthInSamples : currentEndPosition;
        auto startPositionInTicks = track.getProject().secondsToTicks(startPositionInSamples / currentSampleRate);
        auto endPositionInTicks = track.getProject().secondsToTicks(endPositionInSamples / currentSampleRate);
        list<NoteRoll *> noteRollsToPlay = timeline.getAt(startPositionInTicks, endPositionInTicks);
        if (track.isRecording()) {
            noteRollsToPlay.remove(&track.getMidiRecorder()->getNoteRoll());
        }
        bool wrapped = false;
        if (looping) {
            auto newStartPos = currentPosition % sourceLengthInSamples;
            auto newEndPos = (currentPosition + bufferToFill.numSamples) % sourceLengthInSamples;
            if (newEndPos < newStartPos) {
                wrapped = true;
                MidiBuffer midiEvents;
                processMidiBuffer(midiEvents, noteRollsToPlay, newStartPos, sourceLengthInSamples);
                track.getInstrument().renderNextBlock(
                    *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
            }
        }
        if (!wrapped) {
            MidiBuffer midiEvents;
            processMidiBuffer(
                midiEvents, noteRollsToPlay, startPositionInSamples, startPositionInSamples + bufferToFill.numSamples);
            track.getInstrument().renderNextBlock(
                *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
        }
        currentPosition += bufferToFill.numSamples;
    }
}

Timeline<int, NoteRoll *> MidiPlayer::getCurrentTimeline() {
    Timeline<int, NoteRoll *> timeline;
    for (shared_ptr<NoteRoll> &noteRoll : noteRolls) {
        timeline.addRange(noteRoll->getStartPosInTicks(), noteRoll->getEndPosInTicks(), noteRoll.get());
    }
    return timeline;
}

void MidiPlayer::processMidiBuffer(
    MidiBuffer &buffer, const list<NoteRoll *> &noteRollsToPlay, int64 startTimeInSamples, int64 endTimeInSamples) {

    auto startTimeInSeconds = (double)startTimeInSamples / currentSampleRate;
    auto endTimeInSeconds = (double)endTimeInSamples / currentSampleRate;
    auto startTimeInTicks = track.getProject().secondsToTicks(startTimeInSeconds);
    auto endTimeInTicks = track.getProject().secondsToTicks(endTimeInSeconds);
    for (auto *noteRoll : noteRollsToPlay) {
        noteRoll->processNextMidiBuffer(buffer, startTimeInTicks, endTimeInTicks);
    }
}

//==============================================================================
void MidiPlayer::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    currentPosition = newPosition;
}

int64 MidiPlayer::getNextReadPosition() const {
    const ScopedLock lock(mutex);
    return looping ? currentPosition % getTotalLength() : currentPosition;
}

int64 MidiPlayer::getTotalLength() const {
    const ScopedLock lock(mutex);
    int maxLengthInTicks = 0;
    for (auto &noteRoll : noteRolls) {
        if (!noteRoll->isDeleted()) {
            maxLengthInTicks = max(maxLengthInTicks, noteRoll->getEndPosInTicks());
        }
    }
    auto lengthInSeconds = track.getProject().ticksToSeconds(maxLengthInTicks);
    auto lengthInSamples = (int64)(lengthInSeconds * currentSampleRate);

    if (track.isRecording() && !looping) {
        lengthInSamples = max(lengthInSamples, currentPosition);
    }
    return lengthInSamples;
}

bool MidiPlayer::isLooping() const { return looping; }

void MidiPlayer::setLooping(bool shouldLoop) { looping = shouldLoop; }

} // namespace trackman
