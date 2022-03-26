#include "MidiPlayer.h"
#include "Project.h"
#include "Track.h"

namespace trackman {

MidiPlayer::MidiPlayer(Track &track) : track(track), noteRolls(track.getNoteRolls()) {
    for (auto i = 0; i < 4; ++i)
        synth.addVoice(new SineWaveVoice());

    synth.addSound(new SineWaveSound());
}

Timeline<NoteRoll *> MidiPlayer::getCurrentTimeline() {
    Timeline<NoteRoll *> timeline;
    for (shared_ptr<NoteRoll> &noteRoll : noteRolls) {
        timeline.addRange(noteRoll->getStartPosInSeconds(), noteRoll->getEndPosInSeconds(), noteRoll.get());
    }
    return timeline;
}

void MidiPlayer::prepareToPlay(int blockSize, double sampleRate) {
    track.getProject().getKeyboardState().reset();
    {
        const ScopedLock lock(mutex);
        currentSampleRate = sampleRate;
    }
    tempBuffer.setSize(2, sampleRate);
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (auto &noteRoll : noteRolls) {
        noteRoll->prepareToPlay(blockSize, sampleRate);
    }
}

void MidiPlayer::releaseResources() {
    const ScopedLock lock(mutex);
    for (auto &noteRoll : noteRolls) {
        noteRoll->releaseResources();
    }
    tempBuffer.setSize(2, 0);
    currentSampleRate = 0;
}

void MidiPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
//    DBG("MidiPlayer::getNextAudioBlock pos: " << currentPosition << " to: " << currentPosition + bufferToFill.numSamples);
//    DBG("midi player pos: " << currentPosition);
    if (bufferToFill.numSamples > 0) {
        bufferToFill.clearActiveBufferRegion();
        Timeline timeline = getCurrentTimeline();
        auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
        list<NoteRoll *> noteRollsToPlay = timeline.getAt(getTimeAtPosition(pos));
//        if (track.isRecording()) {
//            MidiBuffer incomingMidi;
//            processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, pos);
////
////            for (auto i = incomingMidi.begin(); i != incomingMidi.end(); i++) {
////                auto m = (*i).getMessage();
////                auto t = track.getProject().ticksToSeconds(m.getTimeStamp());
////                DBG(String("note ") + (m.isNoteOn() ? "on" : "off") + " event at time "
////                    << t << " (" << m.getTimeStamp() << " ticks, " << (t * currentSampleRate)
////                    << " samples): noteNumber=" << m.getNoteNumber() << " velocity=" << m.getVelocity());
////            }
////
//            track.getSynth().renderNextBlock(
//                *bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
//            noteRollsToPlay.remove(&track.getMidiRecorder()->getNoteRoll());
//        }
        if (!noteRollsToPlay.empty()) {
            auto firstNoteRoll = noteRollsToPlay.front();
            if (!firstNoteRoll->isDeleted()) {
                firstNoteRoll->getNextAudioBlock(bufferToFill);
            } else {
                bufferToFill.clearActiveBufferRegion();
            }
            if (noteRollsToPlay.size() > 1) {
                tempBuffer.setSize(
                    jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());

                AudioSourceChannelInfo info2(&tempBuffer, 0, bufferToFill.numSamples);
                int i = 0;
                for (auto *noteRoll : noteRollsToPlay) {
                    if (i > 0 && !noteRoll->isDeleted()) {
                        noteRoll->getNextAudioBlock(info2);
                        for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
                            bufferToFill.buffer->addFrom(
                                chan, bufferToFill.startSample, tempBuffer, chan, 0, bufferToFill.numSamples);
                        }
                    }
                    i++;
                }
            }
//        } else {
//            bufferToFill.clearActiveBufferRegion();
        }
        currentPosition += bufferToFill.numSamples;
    }
}

void MidiPlayer::processNextMidiBuffer(
    MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) const {

    MidiBuffer keyboardBuffer;
    auto &keyboardState = track.getProject().getKeyboardState();
    keyboardState.processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
    buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
}

//==============================================================================
void MidiPlayer::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    for (auto &noteRoll : noteRolls) {
        if (!noteRoll->isDeleted()) {
            noteRoll->setNextReadPosition(newPosition);
        }
    }
    currentPosition = newPosition;
}

int64 MidiPlayer::getNextReadPosition() const { return looping ? currentPosition % getTotalLength() : currentPosition; }

int64 MidiPlayer::getTotalLength() const {
    const ScopedLock lock(mutex);
    int64 totalLength = 0;
    for (auto &noteRoll : noteRolls) {
        if (!noteRoll->isDeleted()) {
            totalLength = max(totalLength, noteRoll->getTotalLength());
        }
    }
    if (track.isRecording() && !looping) {
        totalLength = max(totalLength, currentPosition);
    }
//    return totalLength > 0 ? totalLength + 1024 : 0;
    return totalLength;
}

bool MidiPlayer::isLooping() const { return looping; }

void MidiPlayer::setLooping(bool shouldLoop) { looping = shouldLoop; }

} // namespace trackman
