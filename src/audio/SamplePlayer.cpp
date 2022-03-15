#include "SamplePlayer.h"

SamplePlayer::SamplePlayer(std::list<std::shared_ptr<Sample>> &samples) : samples(samples) {}

SamplePlayer::~SamplePlayer() {
    for (auto &sample : samples) {
        sample->releaseResources();
    }
}
//
// std::list<juce::PositionableAudioSource *> SamplePlayer::getSources() {
//    std::list<juce::PositionableAudioSource *> sources;
//    for (auto &sample : samples) {
//        if (sample->getSource() != nullptr) {
//            sources.push_back(sample->getSource());
//        }
//    }
//    return sources;
//}

Timeline<Sample *> SamplePlayer::getCurrentTimeline() {
    Timeline<Sample *> timeline;
    for (std::shared_ptr<Sample> &sample : samples) {
        timeline.addRange(sample->getStartPos(), sample->getEndPos(), sample.get());
    }
    return timeline;
}

//==============================================================================
void SamplePlayer::prepareToPlay(int blockSize, double sampleRate) {
    const juce::ScopedLock lock(mutex);
    currentBlockSize = blockSize;
    currentSampleRate = sampleRate;
    tempBuffer.setSize(2, sampleRate);

    for (auto &sample : samples) {
        sample->prepareToPlay(blockSize, sampleRate);
    }
}

void SamplePlayer::releaseResources() {
    const juce::ScopedLock lock(mutex);
    for (auto &sample : samples) {
        sample->releaseResources();
    }
    tempBuffer.setSize(2, 0);
    currentSampleRate = 0;
    currentBlockSize = 0;
}

void SamplePlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    const juce::ScopedLock lock(mutex);
    if (bufferToFill.numSamples > 0) {
        currentPos += bufferToFill.numSamples;
        Timeline timeline = getCurrentTimeline();
        std::list<Sample *> samplesToPlay = timeline.getAt(getTimeAtPosition(currentPos));
        if (!samplesToPlay.empty()) {
            auto firstSample = samplesToPlay.front();
            if (!firstSample->isDeleted()) {
                firstSample->getNextAudioBlock(bufferToFill);
            } else {
                bufferToFill.clearActiveBufferRegion();
            }

            if (samplesToPlay.size() > 1) {
                tempBuffer.setSize(
                    juce::jmax(1, bufferToFill.buffer->getNumChannels()), bufferToFill.buffer->getNumSamples());

                juce::AudioSourceChannelInfo info2(&tempBuffer, 0, bufferToFill.numSamples);
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
    }
}

//==============================================================================
void SamplePlayer::setNextReadPosition(juce::int64 newPosition) {
    const juce::ScopedLock lock(mutex);
    currentPos = newPosition;
    for (auto &sample : samples) {
        if (!sample->isDeleted()) {
            sample->setNextReadPosition(newPosition);
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
        if (!sample->isDeleted()) {
            totalLength = std::max(totalLength, sample->getLengthInSamples());
        }
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
