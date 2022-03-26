#include "Instrument.h"
#include "Track.h"

namespace trackman {

Instrument::Instrument(Track &track) {
    for (auto i = 0; i < polyphony; ++i) {
        synth.addVoice(new SineWaveVoice());
    }
    synth.addSound(new SineWaveSound());
    synth.setCurrentPlaybackSampleRate(track.getDeviceManager().getAudioDeviceSetup().sampleRate);
}

void Instrument::renderNextBlock(AudioBuffer<float> &outputAudio, const MidiBuffer &inputMidi, int startSample, int numSamples) {
    synth.renderNextBlock(outputAudio, inputMidi, startSample, numSamples);
}

} // namespace trackman
