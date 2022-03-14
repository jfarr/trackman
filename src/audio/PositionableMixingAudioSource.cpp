#include "PositionableMixingAudioSource.h"
#include "PositionableResamplingAudioSource.h"

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource *input) {
    const juce::ScopedLock lock(mutex);
    if (input != nullptr) {
        mixer.addInputSource(input, false);
        inputs.add(input);
    }
}

void PositionableMixingAudioSource::removeInputSource(PositionableAudioSource *input) {
    const juce::ScopedLock lock(mutex);
    if (input != nullptr) {
        mixer.removeInputSource(input);
        const int index = inputs.indexOf(input);
        if (index >= 0) {
            inputs.remove(index);
        }
    }
}

void PositionableMixingAudioSource::removeAllInputs() {
    const juce::ScopedLock lock(mutex);
    mixer.removeAllInputs();
    inputs.clear();
}

void PositionableMixingAudioSource::setTotalLengthSecs(double newLen) {
    const juce::ScopedLock lock(mutex);
    totalLengthSecs = newLen;
    totalLength = newLen * getSampleRate();
}

double PositionableMixingAudioSource::getSampleRate() {
    return sampleRate == 0 ? deviceManager.getAudioDeviceSetup().sampleRate : sampleRate;
}

void PositionableMixingAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    DBG("PositionableMixingAudioSource::prepareToPlay - blocksize: " << blockSize << " sample rate: " << newSampleRate);
    const juce::ScopedLock lock(mutex);
    if (newSampleRate != sampleRate) {
        totalLength = totalLengthSecs * newSampleRate;
        sampleRate = newSampleRate;
    }
    mixer.prepareToPlay(blockSize, newSampleRate);
}

void PositionableMixingAudioSource::releaseResources() { mixer.releaseResources(); }

void PositionableMixingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    const juce::ScopedLock lock(mutex);
    juce::int64 currentPos = getNextReadPosition();
    if (currentPos > totalLength) {
        setNextReadPosition(currentPos);
    }
    mixer.getNextAudioBlock(bufferToFill);
}

void PositionableMixingAudioSource::setNextReadPosition(juce::int64 newPosition) {
    const juce::ScopedLock lock(mutex);
    newPosition = looping ? newPosition % totalLength : newPosition;
    for (int i = inputs.size(); --i >= 0;) {
        inputs.getUnchecked(i)->setNextReadPosition(newPosition);
    }
}

juce::int64 PositionableMixingAudioSource::getNextReadPosition() const {
    const juce::ScopedLock lock(mutex);
    juce::int64 nextPos = 0;
    for (int i = inputs.size(); --i >= 0;) {
        auto pos = inputs.getUnchecked(i)->getNextReadPosition();
        nextPos = juce::jmax(nextPos, pos);
    }
    return nextPos;
}

juce::int64 PositionableMixingAudioSource::getTotalLength() const {
    const juce::ScopedLock lock(mutex);
    return totalLength;
}

bool PositionableMixingAudioSource::isLooping() const {
    const juce::ScopedLock lock(mutex);
    return looping;
}

void PositionableMixingAudioSource::setLooping(bool shouldLoop) {
    const juce::ScopedLock lock(mutex);
    looping = shouldLoop;
}
