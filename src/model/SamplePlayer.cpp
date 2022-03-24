#include "SamplePlayer.h"

namespace trackman {

SamplePlayer::SamplePlayer(list<shared_ptr<Sample>> &samples) : samples(samples) {}

SamplePlayer::~SamplePlayer() {
    for (auto &sample : samples) {
        sample->releaseResources();
    }
}

Timeline<Sample *> SamplePlayer::getCurrentTimeline() {
    Timeline<Sample *> timeline;
    for (shared_ptr<Sample> &sample : samples) {
        timeline.addRange(sample->getStartPosInSeconds(), sample->getEndPosInSeconds(), sample.get());
    }
    return timeline;
}

//==============================================================================
void SamplePlayer::prepareToPlay(int blockSize, double sampleRate) {
    const ScopedLock lock(mutex);
    currentSampleRate = sampleRate;
    tempBuffer.setSize(2, sampleRate);

    for (auto &sample : samples) {
        sample->prepareToPlay(blockSize, sampleRate);
    }
}

void SamplePlayer::releaseResources() {
    const ScopedLock lock(mutex);
    for (auto &sample : samples) {
        sample->releaseResources();
    }
    tempBuffer.setSize(2, 0);
    currentSampleRate = 0;
}

void SamplePlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    const ScopedLock lock(mutex);
    if (bufferToFill.numSamples > 0) {
        Timeline timeline = getCurrentTimeline();
        auto pos = looping ? currentPos % getTotalLength() : currentPos;
        list<Sample *> samplesToPlay = timeline.getAt(getTimeAtPosition(pos));
        if (!samplesToPlay.empty()) {
            auto firstSample = samplesToPlay.front();
            if (!firstSample->isDeleted()) {
                firstSample->getNextAudioBlock(bufferToFill);
            } else {
                bufferToFill.clearActiveBufferRegion();
            }

            if (samplesToPlay.size() > 1) {
                tempBuffer.setSize(
                    jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());

                AudioSourceChannelInfo info2(&tempBuffer, 0, bufferToFill.numSamples);
                int i = 0;
                for (auto *sample : samplesToPlay) {
                    if (i > 0 && !sample->isDeleted()) {
                        sample->getNextAudioBlock(info2);
                        for (int chan = 0; chan < bufferToFill.buffer->getNumChannels(); ++chan) {
                            bufferToFill.buffer->addFrom(
                                chan, bufferToFill.startSample, tempBuffer, chan, 0, bufferToFill.numSamples);
                        }
                    }
                    i++;
                }
            }
        } else {
            bufferToFill.clearActiveBufferRegion();
        }
        currentPos += bufferToFill.numSamples;
    }
}

//==============================================================================
void SamplePlayer::setNextReadPosition(int64 newPosition) {
    const ScopedLock lock(mutex);
    currentPos = newPosition;
    for (auto &sample : samples) {
        if (!sample->isDeleted()) {
            sample->setNextReadPosition(newPosition);
        }
    }
}

int64 SamplePlayer::getNextReadPosition() const {
    const ScopedLock lock(mutex);
    return looping ? currentPos % getTotalLength() : currentPos;
}

int64 SamplePlayer::getTotalLength() const {
    const ScopedLock lock(mutex);
    int64 totalLength = 0;
    for (shared_ptr<Sample> &sample : samples) {
        if (!sample->isDeleted()) {
            totalLength = max(totalLength, sample->getLengthInSamples());
        }
    }
    return totalLength;
}

bool SamplePlayer::isLooping() const {
    const ScopedLock lock(mutex);
    return looping;
}

void SamplePlayer::setLooping(bool shouldLoop) {
    const ScopedLock lock(mutex);
    looping = shouldLoop;
    for (auto &sample : samples) {
        if (!sample->isDeleted()) {
            sample->setLooping(shouldLoop);
        }
    }
}

} // namespace trackman
