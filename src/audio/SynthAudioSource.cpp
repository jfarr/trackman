#include "SynthAudioSource.h"
#include "model/Track.h"

namespace trackman {

void SynthAudioSource::prepareToPlay(int /*blockSize*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
    MidiBuffer incomingMidi;
    track.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, pos);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    currentPosition += bufferToFill.numSamples;
}

void SynthAudioSource::setNextReadPosition(int64 newPosition) { currentPosition = newPosition; }

int64 SynthAudioSource::getNextReadPosition() const {
    return looping ? currentPosition % getTotalLength() : currentPosition;
}

int64 SynthAudioSource::getTotalLength() const {
    int64 len = track.getMidiLengthInSamples();
    return track.isRecording() && !looping ? max(len, currentPosition) : len;
}

bool SynthAudioSource::isLooping() const { return looping; }

void SynthAudioSource::setLooping(bool shouldLoop) { looping = shouldLoop; }

} // namespace trackman
