#include "InstrumentPlayer.h"
#include "Project.h"

namespace trackman {

InstrumentPlayer::InstrumentPlayer(Project &project)
    : MidiHandler(project), startTime(Time::getMillisecondCounterHiRes() * 0.001) {
    onMidiMessage = [this](const MidiMessage &message, const double time) { midiMessageReceived(message, time); };
}

void InstrumentPlayer::midiMessageReceived(const MidiMessage &message, const double time) {
    DBG("midi " << (message.isNoteOn() ? "on" : "off") << " at " << time << " note: " << message.getNoteNumber());
    MidiMessage newMessage = message;
    newMessage.setTimeStamp (juce::Time::getMillisecondCounterHiRes() * 0.001);
    midiCollector.addMessageToQueue(newMessage);
}

void InstrumentPlayer::prepareToPlay(int blockSize, double sampleRate) {
    midiCollector.reset(sampleRate);
}

void InstrumentPlayer::releaseResources() {}

void InstrumentPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    juce::MidiBuffer incomingMidi;
    midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);

//    project.getKeyboardState().processNextMidiBuffer(
//        incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);

    auto *synth = project.getLiveSynth();
    if (synth != nullptr) {
        synth->renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }
}

} // namespace trackman
