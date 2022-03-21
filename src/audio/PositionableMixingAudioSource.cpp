#include "PositionableMixingAudioSource.h"

namespace trackman {

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource *input) {
    const ScopedLock lock(mutex);
    if (input != nullptr) {
        mixer.addInputSource(input, false);
        inputs.add(input);
    }
}

void PositionableMixingAudioSource::removeInputSource(PositionableAudioSource *input) {
    const ScopedLock lock(mutex);
    if (input != nullptr) {
        mixer.removeInputSource(input);
        const int index = inputs.indexOf(input);
        if (index >= 0) {
            inputs.remove(index);
        }
    }
}

void PositionableMixingAudioSource::removeAllInputs() {
    const ScopedLock lock(mutex);
    mixer.removeAllInputs();
    inputs.clear();
}

void PositionableMixingAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    mixer.prepareToPlay(blockSize, newSampleRate);
}

void PositionableMixingAudioSource::releaseResources() { mixer.releaseResources(); }

void PositionableMixingAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    mixer.getNextAudioBlock(bufferToFill);
}

void PositionableMixingAudioSource::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    newPosition = looping ? newPosition % getTotalLength() : newPosition;
    for (int i = inputs.size(); --i >= 0;) {
        inputs.getUnchecked(i)->setNextReadPosition(newPosition);
    }
}

int64 PositionableMixingAudioSource::getNextReadPosition() const {
    const ScopedLock lock(mutex);
    int64 nextPos = 0;
    for (int i = inputs.size(); --i >= 0;) {
        auto pos = inputs.getUnchecked(i)->getNextReadPosition();
        nextPos = jmax(nextPos, pos);
    }
    return nextPos;
}

int64 PositionableMixingAudioSource::getTotalLength() const {
    const ScopedLock lock(mutex);
    int64 totalLength = 0;
    for (int i = inputs.size(); --i >= 0;) {
        auto inputLength = inputs.getUnchecked(i)->getTotalLength();
        if (inputLength > totalLength) {
            totalLength = inputLength;
        }
    }
    return totalLength;
}

bool PositionableMixingAudioSource::isLooping() const {
    const ScopedLock lock(mutex);
    return looping;
}

void PositionableMixingAudioSource::setLooping(bool shouldLoop) {
    const ScopedLock lock(mutex);
    looping = shouldLoop;
    for (int i = inputs.size(); --i >= 0;) {
        inputs.getUnchecked(i)->setLooping(shouldLoop);
    }
}

} // namespace trackman
