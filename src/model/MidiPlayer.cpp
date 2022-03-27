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
    DBG("MidiPlayer::getNextAudioBlock start: " << bufferToFill.startSample << " num: " << bufferToFill.numSamples);
    if (bufferToFill.numSamples > 0) {
        Timeline timeline = getCurrentTimeline();
        auto positionInSamples = looping ? currentPosition % getTotalLength() : currentPosition;
        auto positionInTicks = track.getProject().secondsToTicks(positionInSamples / currentSampleRate);
        DBG("position: " << positionInSamples << " (" << positionInTicks << " ticks)");
        list<NoteRoll *> noteRollsToPlay = timeline.getAt(positionInTicks);
        if (track.isRecording()) {
            noteRollsToPlay.remove(&track.getMidiRecorder()->getNoteRoll());
        }
        if (looping) {
            //            auto sourceLengthInSamples = (int64)(getLengthInSeconds() * currentSampleRate);
            //            auto newStartPos = currentPosition % sourceLengthInSamples;
            //            auto newEndPos = (currentPosition + bufferToFill.numSamples) % sourceLengthInSamples;
            //            if (newEndPos < newStartPos) {
            //                DBG("WRAPPED!!!!!!");
            //                MidiBuffer midiEvents1;
            //                processMidiBuffer(midiEvents1, newStartPos, sourceLengthInSamples);
            //                MidiBuffer midiEvents2;
            //                processMidiBuffer(midiEvents2, 0, newEndPos);
            //            } else {
            //                MidiBuffer midiEvents;
            //                processMidiBuffer(midiEvents, newStartPos, newEndPos);
            //            }
        } else {
            MidiBuffer midiEvents;
            processMidiBuffer(midiEvents, noteRollsToPlay, currentPosition, currentPosition + bufferToFill.numSamples);
            track.getInstrument().renderNextPlaybackBlock(
                *bufferToFill.buffer, midiEvents, bufferToFill.startSample, bufferToFill.numSamples);
        }
        currentPosition += bufferToFill.numSamples;
    }

    //    if (bufferToFill.numSamples > 0) {
    //        Timeline timeline = getCurrentTimeline();
    //        auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
    //        list<NoteRoll *> noteRollsToPlay = timeline.getAt(getTimeAtPosition(pos));
    //        if (track.isRecording()) {
    //            noteRollsToPlay.remove(&track.getMidiRecorder()->getNoteRoll());
    //        }
    //        if (!noteRollsToPlay.empty()) {
    //            //            track.getInstrument().renderNextPlaybackBlock(
    //            //                *bufferToFill.buffer, noteRollsToPlay, bufferToFill.startSample,
    //            bufferToFill.numSamples,
    //            //                pos);
    //            auto firstNoteRoll = noteRollsToPlay.front();
    //            if (!firstNoteRoll->isDeleted()) {
    //                firstNoteRoll->getNextAudioBlock(bufferToFill);
    //            } else {
    //                bufferToFill.clearActiveBufferRegion();
    //            }
    //
    //            if (noteRollsToPlay.size() > 1) {
    //                tempBuffer.setSize(
    //                    jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());
    //
    //                AudioSourceChannelInfo info2(&tempBuffer, 0, bufferToFill.numSamples);
    //                int i = 0;
    //                for (auto *noteRoll : noteRollsToPlay) {
    //                    if (i > 0 && !noteRoll->isDeleted()) {
    //                        noteRoll->getNextAudioBlock(info2);
    //                        for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
    //                            bufferToFill.buffer->addFrom(
    //                                chan, bufferToFill.startSample, tempBuffer, chan, 0, bufferToFill.numSamples);
    //                        }
    //                    }
    //                    i++;
    //                }
    //            }
    //        } else {
    //            bufferToFill.clearActiveBufferRegion();
    //        }
    //        currentPosition += bufferToFill.numSamples;
    //    }
}

void MidiPlayer::processMidiBuffer(
    MidiBuffer &buffer, list<NoteRoll *> noteRollsToPlay, int64 startTimeInSamples, int64 endTimeInSamples) {

    for (auto *noteRoll : noteRollsToPlay) {
        noteRoll->processNextMidiBuffer(buffer, startTimeInSamples, endTimeInSamples);
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
