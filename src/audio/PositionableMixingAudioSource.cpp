#include "PositionableMixingAudioSource.h"
#include "PositionableResamplingAudioSource.h"

PositionableMixingAudioSource::PositionableMixingAudioSource(double sampleRate) : sampleRate(sampleRate) {
}

PositionableMixingAudioSource::~PositionableMixingAudioSource() { removeAllInputs(); }

void PositionableMixingAudioSource::addInputSource(PositionableAudioSource *input, const bool deleteWhenRemoved,
    double sourceSampleRateToCorrectFor, int maxNumChannels) {
    jassert (input != nullptr);
    if (sourceSampleRateToCorrectFor > 0) {
        input = new PositionableResamplingAudioSource(
            input, deleteWhenRemoved, sampleRate, sourceSampleRateToCorrectFor, maxNumChannels);
        mixer.addInputSource(input, true);
    } else {
        mixer.addInputSource(input, deleteWhenRemoved);
    }
    inputs.add(input);
}

void PositionableMixingAudioSource::removeInputSource(PositionableAudioSource *input) {
    if (input != nullptr) {
        for (int i = inputs.size(); --i >= 0;) {
            PositionableAudioSource *thisInput = inputs.getUnchecked(i);
            PositionableResamplingAudioSource *wrapper = dynamic_cast<PositionableResamplingAudioSource *>(thisInput);
            if (wrapper != nullptr && wrapper->getSource() == input) {
                mixer.removeInputSource(thisInput);
                inputs.remove(i);
                break;
            }
        }
        mixer.removeInputSource(input);
        const int index = inputs.indexOf(input);
        if (index >= 0) {
            inputs.remove(index);
        }
    }
}

void PositionableMixingAudioSource::removeAllInputs() {
    inputs.clear();
    mixer.removeAllInputs();
}

void PositionableMixingAudioSource::prepareToPlay(int blockSize, double newSampleRate) {
    DBG("PositionableMixingAudioSource::prepareToPlay - blocksize: " << blockSize << " sample rate: " << newSampleRate);
    sampleRate = newSampleRate;
    mixer.prepareToPlay(blockSize, sampleRate);
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
