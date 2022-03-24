#include "Sample.h"

#include <memory>

namespace trackman {

Sample::Sample(File file, double startPosInSeconds, double endPosInSeconds)
    : file(move(file)), startPosInSeconds(startPosInSeconds), endPosInSeconds(endPosInSeconds),
      lengthInSeconds(endPosInSeconds - startPosInSeconds) {}

Sample::~Sample() {
    if (resamplingSource != nullptr) {
        resamplingSource->releaseResources();
    }
}

int64 Sample::getLengthInSamples() const {
    return resamplingSource == nullptr ? 0
                                       : getPositionFromTime(startPosInSeconds) + resamplingSource->getTotalLength();
}

int64 Sample::getPositionFromTime(double t) const {
    return resamplingSource == nullptr ? 0 : t * resamplingSource->getSampleRate();
}

double Sample::getSampleRate() const {
    return reader->sampleRate * sourceLengthInSeconds / (endPosInSeconds - startPosInSeconds);
}

void Sample::loadFile(AudioDeviceManager &deviceManager, AudioFormatManager &formatManager) {
    auto blockSize = deviceManager.getAudioDeviceSetup().bufferSize;
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    reader.reset(formatManager.createReaderFor(file));
    if (reader != nullptr) {
        fileSource = make_unique<AudioFormatReaderSource>(reader.get(), false);
        sourceLengthInSeconds = reader->lengthInSamples / reader->sampleRate;
        resamplingSource =
            make_unique<PositionableResamplingAudioSource>(fileSource.get(), false, sampleRate, getSampleRate(), 2);
        resamplingSource->prepareToPlay(blockSize, sampleRate);
        loaded = true;
    }
}

void Sample::setPosition(double newPosInSeconds) {
    startPosInSeconds = newPosInSeconds;
    endPosInSeconds = startPosInSeconds + lengthInSeconds;
}

void Sample::setLength(double newLengthInSeconds) {
    lengthInSeconds = newLengthInSeconds;
    endPosInSeconds = startPosInSeconds + newLengthInSeconds;
    if (resamplingSource != nullptr) {
        resamplingSource->setSourceSampleRateToCorrectFor(getSampleRate());
    }
}

//==============================================================================
void Sample::prepareToPlay(int blockSize, double sampleRate) {
    if (resamplingSource != nullptr) {
        resamplingSource->prepareToPlay(blockSize, sampleRate);
    }
}

void Sample::releaseResources() {
    if (resamplingSource != nullptr) {
        resamplingSource->releaseResources();
    }
}

void Sample::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    if (resamplingSource != nullptr) {
        resamplingSource->getNextAudioBlock(bufferToFill);
    }
}

//==============================================================================
void Sample::setNextReadPosition(int64 newPosition) {
    if (resamplingSource != nullptr) {
        resamplingSource->setNextReadPosition(newPosition);
    }
}

int64 Sample::getNextReadPosition() const {
    return resamplingSource == nullptr ? 0 : resamplingSource->getNextReadPosition();
}

int64 Sample::getTotalLength() const { return resamplingSource == nullptr ? 0 : resamplingSource->getTotalLength(); }

bool Sample::isLooping() const { return false; }

void Sample::setLooping(bool shouldLoop) {
    if (resamplingSource != nullptr) {
        resamplingSource->setLooping(shouldLoop);
    }
}

} // namespace trackman
