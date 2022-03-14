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

void PositionableMixingAudioSource::setTotalLengthSecs(double newLen) {
    DBG("PositionableMixingAudioSource::setTotalLengthSecs: " << newLen);
    const juce::ScopedLock lock(mutex);
    totalLengthSecs = newLen;
    totalLength = newLen * getSampleRate();
    if (totalLength <= 0) {
        DBG("wtf mate");
    } else if (totalLength > 1000000) {
        DBG("wtf mate");
    }
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
    if (totalLength <= 0) {
        DBG("wtf mate");
    } else if (totalLength > 1000000) {
        DBG("wtf mate");
    }
    mixer.prepareToPlay(blockSize, newSampleRate);
}

void PositionableMixingAudioSource::releaseResources() { mixer.releaseResources(); }

void PositionableMixingAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (totalLength <= 0) {
        DBG("wtf mate");
    } else if (totalLength > 1000000) {
        DBG("wtf mate");
    }
    juce::int64 currentPos = getNextReadPosition();
    if (currentPos > totalLength) {
        setNextReadPosition(currentPos);
    }
    mixer.getNextAudioBlock(bufferToFill);
}

void PositionableMixingAudioSource::setNextReadPosition(juce::int64 newPosition) {
    newPosition = looping ? newPosition % totalLength : newPosition;
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
    //    juce::int64 totalLength = 0;
    //    for (int i = inputs.size(); --i >= 0;) {
    //        auto inputLength = inputs.getUnchecked(i)->getTotalLength();
    ////        DBG("PositionableMixingAudioSource @" + juce::String::toHexString((long)this) << " got length: " <<
    /// inputLength);
    //        if (inputLength > totalLength) {
    //            totalLength = inputLength;
    //        }
    //    }
    const juce::ScopedLock lock(mutex);
    //    DBG("PositionableMixingAudioSource total length: " << totalLength);
    if (totalLength <= 0) {
        DBG("wtf mate");
    }
    return totalLength;
}

bool PositionableMixingAudioSource::isLooping() const { return looping; }

void PositionableMixingAudioSource::setLooping(bool shouldLoop) { looping = shouldLoop; }
