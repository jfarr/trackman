#include "MidiPlayer.h"
#include "Project.h"
#include "Track.h"

namespace trackman {

void MidiPlayer::prepareToPlay(int /*blockSize*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void MidiPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
    MidiBuffer incomingMidi;
    track.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, pos);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    currentPosition += bufferToFill.numSamples;
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
