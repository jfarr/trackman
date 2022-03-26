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
    if (bufferToFill.numSamples > 0) {
        Timeline timeline = getCurrentTimeline();
        auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
        list<NoteRoll *> noteRollsToPlay = timeline.getAt(getTimeAtPosition(pos));
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
        }
    } else {
        bufferToFill.clearActiveBufferRegion();
    }
    currentPosition += bufferToFill.numSamples;
}

void MidiPlayer::processNextMidiBuffer(
    MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) const {

    MidiBuffer keyboardBuffer;
    auto &keyboardState = track.getProject().getKeyboardState();
    keyboardState.processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
    buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
}

void MidiPlayer::setNextReadPosition(int64 newPosition) { currentPosition = newPosition; }

int64 MidiPlayer::getNextReadPosition() const {
    return looping ? currentPosition % getTotalLength() : currentPosition;
}

int64 MidiPlayer::getTotalLength() const {
    int64 len = track.getMidiLengthInSamples();
    return track.isRecording() && !looping ? max(len, currentPosition) : len;
}

bool MidiPlayer::isLooping() const { return looping; }

void MidiPlayer::setLooping(bool shouldLoop) { looping = shouldLoop; }

} // namespace trackman
