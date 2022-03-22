#include "MidiPlayer.h"

namespace trackman {

void MidiPlayer::prepareToPlay(int blockSize, double sampleRate) {
    synthAudioSource.prepareToPlay(blockSize, sampleRate);
}

void MidiPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    synthAudioSource.getNextAudioBlock(bufferToFill);
}

} // namespace trackman
