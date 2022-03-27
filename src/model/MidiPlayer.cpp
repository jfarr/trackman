#include "MidiPlayer.h"
#include "Project.h"
#include "Track.h"

namespace trackman {

MidiPlayer::MidiPlayer(Track &track) : track(track), noteRolls(track.getNoteRolls()) {
    //    for (auto i = 0; i < 4; ++i) {
    //        synth.addVoice(new SineWaveVoice());
    //    }
    //    synth.addSound(new SineWaveSound());
}

Timeline<int64, NoteRoll *> MidiPlayer::getCurrentTimeline() {
    Timeline<int64, NoteRoll *> timeline;
    for (shared_ptr<NoteRoll> &noteRoll : noteRolls) {
        timeline.addRange(noteRoll->getStartPosInTicks(), noteRoll->getEndPosInTicks(), noteRoll.get());
    }
    return timeline;
}

void MidiPlayer::prepareToPlay(int blockSize, double sampleRate) {
    {
        const ScopedLock lock(mutex);
        currentSampleRate = sampleRate;
    }
    tempBuffer.setSize(2, sampleRate);
    //    for (auto &noteRoll : noteRolls) {
    //        noteRoll->prepareToPlay(blockSize, sampleRate);
    //    }
}

void MidiPlayer::releaseResources() {
    const ScopedLock lock(mutex);
    //    for (auto &noteRoll : noteRolls) {
    //        noteRoll->releaseResources();
    //    }
    tempBuffer.setSize(2, 0);
    currentSampleRate = 0;
}

void MidiPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
    auto currentEndPosition = currentPosition + bufferToFill.numSamples;
    DBG("MidiPlayer::getNextAudioBlock start: " << bufferToFill.startSample << " num: " << bufferToFill.numSamples);
    if (bufferToFill.numSamples > 0) {
        Timeline timeline = getCurrentTimeline();
        auto sourceLengthInSamples = getTotalLength();
        auto startPositionInSamples = looping ? currentPosition % sourceLengthInSamples : currentPosition;
        auto endPositionInSamples = looping ? currentEndPosition % sourceLengthInSamples : currentEndPosition;
        auto startPositionInTicks = track.getProject().secondsToTicks(startPositionInSamples / currentSampleRate);
        auto endPositionInTicks = track.getProject().secondsToTicks(endPositionInSamples / currentSampleRate);
        auto sourceLengthInTicks = track.getProject().secondsToTicks(sourceLengthInSamples / currentSampleRate);
        DBG("startPositionInSamples: " << startPositionInSamples << " (" << startPositionInTicks << " of "
                                       << sourceLengthInTicks << " ticks)");
        DBG("endPositionInSamples: " << endPositionInSamples << " (" << endPositionInTicks << " of "
                                       << sourceLengthInTicks << " ticks)");
        list<NoteRoll *> noteRollsToPlay = timeline.getAt(startPositionInTicks, endPositionInTicks);
        if (track.isRecording()) {
            noteRollsToPlay.remove(&track.getMidiRecorder()->getNoteRoll());
        }
        DBG("note rolls: " << noteRollsToPlay.size());
        bool wrapped = false;
        if (looping) {
            auto newStartPos = currentPosition % sourceLengthInSamples;
            auto newEndPos = (currentPosition + bufferToFill.numSamples) % sourceLengthInSamples;
            if (newEndPos < newStartPos) {
                DBG("WRAPPED!!!!!!");
                wrapped = true;
                MidiBuffer midiEvents;
                processMidiBuffer(midiEvents, noteRollsToPlay, newStartPos, sourceLengthInSamples);
                auto len1 = sourceLengthInSamples - newStartPos;
                auto newStartSample = bufferToFill.numSamples - len1;
                DBG("playing " << len1 << " samples starting from " << newStartSample);
                track.getInstrument().renderNextPlaybackBlock(
                    *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
                //                MidiBuffer midiEvents2;
                //                processMidiBuffer(midiEvents2, noteRollsToPlay, 0, newEndPos);
                //                DBG("playing " << newEndPos << " samples starting from 0");
                //                track.getInstrument().renderNextPlaybackBlock(*bufferToFill.buffer, midiEvents2, 0,
                //                newStartSample);
            }
        }
        if (!wrapped) {
            MidiBuffer midiEvents;
            processMidiBuffer(
                midiEvents, noteRollsToPlay, startPositionInSamples, startPositionInSamples + bufferToFill.numSamples);
            track.getInstrument().renderNextPlaybackBlock(
                *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
        }
        currentPosition += bufferToFill.numSamples;
    }
}

void MidiPlayer::processMidiBuffer(
    MidiBuffer &buffer, list<NoteRoll *> noteRollsToPlay, int64 startTimeInSamples, int64 endTimeInSamples) {

    auto startTimeInSeconds = (double)(startTimeInSamples / currentSampleRate);
    auto endTimeInSeconds = (double)(endTimeInSamples / currentSampleRate);
    auto startTimeInTicks = track.getProject().secondsToTicks(startTimeInSeconds);
    auto endTimeInTicks = track.getProject().secondsToTicks(endTimeInSeconds);
    for (auto *noteRoll : noteRollsToPlay) {
        noteRoll->processNextMidiBuffer(buffer, startTimeInTicks, endTimeInTicks);
    }
}

//==============================================================================
void MidiPlayer::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    //    for (auto &noteRoll : noteRolls) {
    //        if (!noteRoll->isDeleted()) {
    //            noteRoll->setNextReadPosition(newPosition);
    //        }
    //    }
    currentPosition = newPosition;
}

int64 MidiPlayer::getNextReadPosition() const { return looping ? currentPosition % getTotalLength() : currentPosition; }

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
    //    return totalLength > 0 ? totalLength + 1024 : 0;
    return lengthInSamples;
}

bool MidiPlayer::isLooping() const { return looping; }

void MidiPlayer::setLooping(bool shouldLoop) {
    looping = shouldLoop;
    //    for (auto &noteRoll : noteRolls) {
    //        if (!noteRoll->isDeleted()) {
    //            noteRoll->setLooping(shouldLoop);
    //        }
    //    }
}

} // namespace trackman
