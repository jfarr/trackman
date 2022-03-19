#include "SynthAudioSource.h"
#include "model/Track.h"

void SynthAudioSource::prepareToPlay(int /*blockSize*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    auto pos = looping ? currentPosition % getTotalLength() : currentPosition;
    juce::MidiBuffer incomingMidi;
    track.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, pos);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    currentPosition += bufferToFill.numSamples;
}

void SynthAudioSource::setNextReadPosition(juce::int64 newPosition) { currentPosition = newPosition; }

juce::int64 SynthAudioSource::getNextReadPosition() const {
    return looping ? currentPosition % getTotalLength() : currentPosition;
}

juce::int64 SynthAudioSource::getTotalLength() const {
    juce::int64 len = track.getMidiLengthInSamples();
    return track.isRecording() && !looping ? std::max(len, currentPosition) : len;
}

bool SynthAudioSource::isLooping() const {
    return looping;
}

void SynthAudioSource::setLooping(bool shouldLoop) {
    looping = shouldLoop;
}
