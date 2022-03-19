#include "SynthAudioSource.h"
#include "model/Track.h"


void SynthAudioSource::prepareToPlay(int /*blockSize*/, double sampleRate) {
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void SynthAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    track.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, currentPosition);
    synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    currentPosition += bufferToFill.numSamples;
}

juce::int64 SynthAudioSource::getTotalLength() const {
    return track.getDeviceManager().getAudioDeviceSetup().sampleRate * track.getMidiMessages().getEndTime();
}
