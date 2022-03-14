#include "SamplePlayer.h"

//==============================================================================
void SamplePlayer::prepareToPlay(int blockSize, double sampleRate) {
    const juce::ScopedLock lock(mutex);
    currentBlockSize = blockSize;
    currentSampleRate = sampleRate;
}

void SamplePlayer::releaseResources() {}

void SamplePlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    const juce::ScopedLock lock(mutex);
    if (bufferToFill.numSamples > 0) {
        currentPos += bufferToFill.numSamples;
    }
}

//==============================================================================
void SamplePlayer::setNextReadPosition(juce::int64 newPosition) {
    const juce::ScopedLock lock(mutex);
    currentPos = newPosition;
}

juce::int64 SamplePlayer::getNextReadPosition() const {
    const juce::ScopedLock lock(mutex);
    return currentPos;
}

juce::int64 SamplePlayer::getTotalLength() const {
    const juce::ScopedLock lock(mutex);
    juce::int64 totalLength = 0;
    for (std::shared_ptr<Sample> &sample : samples) {

    }
    return totalLength;
}

bool SamplePlayer::isLooping() const {
    const juce::ScopedLock lock(mutex);
    return looping;
}

void SamplePlayer::setLooping(bool shouldLoop) {
    const juce::ScopedLock lock(mutex);
    looping = shouldLoop;
}
