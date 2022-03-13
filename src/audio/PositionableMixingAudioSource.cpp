#include "PositionableMixingAudioSource.h"
#include "PositionableResamplingAudioSource.h"

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource *input) {
    if (input != nullptr) {
        mixer.addInputSource(input, false);
        inputs.add(input);
    }
}

void PositionableMixingAudioSource::removeInputSource(PositionableAudioSource *input) {
    if (input != nullptr) {
        mixer.removeInputSource(input);
        const int index = inputs.indexOf(input);
        if (index >= 0) {
            inputs.remove(index);
        }
    }
}

void PositionableMixingAudioSource::removeAllInputs() {
    DBG("PositionableMixingAudioSource::removeAllInputs");
    mixer.removeAllInputs();
    inputs.clear();
}

void PositionableMixingAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    DBG("PositionableMixingAudioSource::prepareToPlay - blocksize: " << blockSize << " sample rate: " << newSampleRate);
    mixer.prepareToPlay(blockSize, newSampleRate);
}

void PositionableMixingAudioSource::releaseResources() { mixer.releaseResources(); }

void PositionableMixingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    juce::int64 currentPos = getNextReadPosition();
    if (currentPos > getTotalLength()) {
        setNextReadPosition(currentPos);
    }
    mixer.getNextAudioBlock(bufferToFill);
}

void PositionableMixingAudioSource::setNextReadPosition(juce::int64 newPosition) {
    newPosition = looping ? newPosition % getTotalLength() : newPosition;
    for (int i = inputs.size(); --i >= 0;) {
        inputs.getUnchecked(i)->setNextReadPosition(newPosition);
    }
}

juce::int64 PositionableMixingAudioSource::getNextReadPosition() const {
    juce::int64 nextPos = 0;
    for (int i = inputs.size(); --i >= 0;) {
        auto pos = inputs.getUnchecked(i)->getNextReadPosition();
        nextPos = juce::jmax(nextPos, pos);
    }
    return nextPos;
}

juce::int64 PositionableMixingAudioSource::getTotalLength() const {
    juce::int64 totalLength = 0;
    for (int i = inputs.size(); --i >= 0;) {
        auto inputLength = inputs.getUnchecked(i)->getTotalLength();
        if (inputLength > totalLength) {
            totalLength = inputLength;
        }
    }
    return totalLength;
}

bool PositionableMixingAudioSource::isLooping() const { return looping; }

void PositionableMixingAudioSource::setLooping(bool shouldLoop) { looping = shouldLoop; }
