#include "SamplePlayer.h"

//==============================================================================
void SamplePlayer::prepareToPlay(int blockSize, double sampleRate) {
    const juce::ScopedLock lock(mutex);
    currentBlockSize = blockSize;
    currentSampleRate = sampleRate;
    tempBuffer.setSize (2, sampleRate);

    for (std::shared_ptr<Sample> &sample : samples) {
        if (sample->getSource() != nullptr) {
            sample->getSource()->prepareToPlay(blockSize, sampleRate);
        }
    }
}

void SamplePlayer::releaseResources() {
    const juce::ScopedLock lock(mutex);
    for (std::shared_ptr<Sample> &sample : samples) {
        if (sample->getSource() != nullptr) {
            sample->getSource()->releaseResources();
        }
    }
    tempBuffer.setSize (2, 0);
    currentSampleRate = 0;
    currentBlockSize = 0;
}

void SamplePlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    const juce::ScopedLock lock(mutex);
    if (bufferToFill.numSamples > 0) {
        currentPos += bufferToFill.numSamples;
        updateTimeline();
        std::list<Sample *> samplesToPlay = timeline.getAt(getTimeAtPosition(currentPos));
        for (auto iter = samplesToPlay.begin(); iter != samplesToPlay.end(); iter++) {
            if ((*iter)->getSource() == nullptr) {
                samplesToPlay.erase(iter);
            }
        }
        if (!samplesToPlay.empty()) {
            samplesToPlay.front()->getSource()->getNextAudioBlock(bufferToFill);

            if (samplesToPlay.size() > 1) {
                tempBuffer.setSize(
                    juce::jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());

                juce::AudioSourceChannelInfo info2(&tempBuffer, 0, bufferToFill.numSamples);
                int i = 0;
                for (Sample *sample : samplesToPlay) {
                    if (i > 0) {
                        sample->getSource()->getNextAudioBlock(info2);
                        for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan)
                            bufferToFill.buffer->addFrom(
                                chan, bufferToFill.startSample, tempBuffer, chan, 0, bufferToFill.numSamples);
                    }
                    i++;
                }
            }
        } else {
            bufferToFill.clearActiveBufferRegion();
        }
    }
}

//==============================================================================
void SamplePlayer::setNextReadPosition(juce::int64 newPosition) {
    const juce::ScopedLock lock(mutex);
    currentPos = newPosition;
    for (std::shared_ptr<Sample> &sample : samples) {
        if (sample->getSource() != nullptr) {
            sample->getSource()->setNextReadPosition(newPosition);
        }
    }
}

juce::int64 SamplePlayer::getNextReadPosition() const {
    const juce::ScopedLock lock(mutex);
    return currentPos;
}

juce::int64 SamplePlayer::getTotalLength() const {
    const juce::ScopedLock lock(mutex);
    juce::int64 totalLength = 0;
    for (std::shared_ptr<Sample> &sample : samples) {
        //        DBG("got length " << sample->getLengthInSamples() << " for sample " <<
        //        sample->getFile().getFileName());
        totalLength = std::max(totalLength, sample->getLengthInSamples());
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

void SamplePlayer::updateTimeline() {
    const juce::ScopedLock lock(mutex);
    timeline.reset();
    for (std::shared_ptr<Sample> &sample : samples) {
        timeline.addRange(sample->getStartPos(), sample->getEndPos(), sample.get());
    }
}
