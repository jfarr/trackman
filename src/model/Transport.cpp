#include "Transport.h"

namespace trackman {

Transport::Transport(PositionableAudioSource &source) {
    transportSource.setSource(&source);
    startTimer(20);
}

void Transport::prepareToPlay(int blockSize, double sampleRate) {
    transportSource.prepareToPlay(blockSize, sampleRate);
}

void Transport::releaseResources() { transportSource.releaseResources(); }

void Transport::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    transportSource.getNextAudioBlock(bufferToFill);
}

void Transport::timerCallback() {}

} // namespace trackman
