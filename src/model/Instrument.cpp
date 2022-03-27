#include "Instrument.h"
#include "NoteRoll.h"
#include "Track.h"

namespace trackman {

Instrument::Instrument(Track &track) : track(track) {
    initSynth(liveSynth);
    initSynth(playbackSynth);
    //    for (auto i = 0; i < polyphony; ++i) {
    //        synth.addVoice(new SineWaveVoice());
    //    }
    //    synth.addSound(new SineWaveSound());
    //    synth.setCurrentPlaybackSampleRate(track.getDeviceManager().getAudioDeviceSetup().sampleRate);
}

void Instrument::initSynth(Synthesiser &synth) {
    for (auto i = 0; i < polyphony; ++i) {
        synth.addVoice(new SineWaveVoice());
    }
    synth.addSound(new SineWaveSound());
    DBG("setCurrentPlaybackSampleRate");
    synth.setCurrentPlaybackSampleRate(track.getDeviceManager().getAudioDeviceSetup().sampleRate);
}
//
//void Instrument::renderNextPlaybackBlock(AudioBuffer<float> &outputAudio, list<NoteRoll *> noteRollsToPlay,
//    int startSample, int numSamples, const int64 currentPos) {
//    MidiBuffer buffer;
//    for (auto *noteRoll : noteRollsToPlay) {
//        MidiBuffer tempBuffer;
//        noteRoll->processNextMidiBuffer(tempBuffer, startSample, numSamples, currentPos);
//        buffer.addEvents(tempBuffer, startSample, numSamples, 0);
//    }
//    renderNextBlock(outputAudio, buffer, startSample, numSamples);
//}

void Instrument::renderNextBlock(
    AudioBuffer<float> &outputAudio, const MidiBuffer &inputMidi, int startSample, int numSamples) {
    playbackSynth.renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
}

} // namespace trackman
