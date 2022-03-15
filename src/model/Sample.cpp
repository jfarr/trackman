#include "Sample.h"

#include <memory>

//#include "Track.h"

Sample::Sample(juce::File file, double startPos, double endPos)
    : file(std::move(file)), startPos(startPos), endPos(endPos), length(endPos - startPos) {}

Sample::~Sample() {
    if (resamplingSource != nullptr) {
        resamplingSource->releaseResources();
    }
}

juce::int64 Sample::getPositionFromTime(double t) const {
    return resamplingSource == nullptr ? 0 : t * resamplingSource->getSampleRate();
}

juce::int64 Sample::getLengthInSamples() const {
    return resamplingSource == nullptr ? 0 : getPositionFromTime(startPos) + resamplingSource->getTotalLength();
}

void Sample::loadFile(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    auto blockSize = deviceManager.getAudioDeviceSetup().bufferSize;
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    reader.reset(formatManager.createReaderFor(file));
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader.get(), false);
        //        offsetSource = std::make_unique<OffsetAudioSource>(*fileSource, startPos, sourceSampleRate);
//        sourceSampleRate = reader->sampleRate;
        resamplingSource = std::make_unique<PositionableResamplingAudioSource>(
            fileSource.get(), false, sampleRate, getSampleRate(), 2);
        resamplingSource->prepareToPlay(blockSize, sampleRate);
        sourceLengthInSeconds = reader->lengthInSamples / reader->sampleRate;
    }
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    //    if (offsetSource != nullptr) {
    //        offsetSource->setOffsetSeconds(startPos);
    //    }
    //    track->updateLength();
}

void Sample::setLength(double newLength) {
    length = newLength;
    endPos = startPos + newLength;
    if (resamplingSource != nullptr) {
        resamplingSource->setSourceSampleRateToCorrectFor(getSampleRate());
    }
    //    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    //    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
    //    track->updateLength();
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

void Sample::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (resamplingSource != nullptr) {
        resamplingSource->getNextAudioBlock(bufferToFill);
    }
}

//==============================================================================
void Sample::setNextReadPosition(juce::int64 newPosition) {
    if (resamplingSource != nullptr) {
        resamplingSource->setNextReadPosition(newPosition);
    }
}

juce::int64 Sample::getNextReadPosition() const {
    return resamplingSource == nullptr ? 0 : resamplingSource->getNextReadPosition();
}

juce::int64 Sample::getTotalLength() const {
    return resamplingSource == nullptr ? 0 : resamplingSource->getTotalLength();
}

bool Sample::isLooping() const {
    return false;
}

void Sample::setLooping(bool shouldLoop) {
}

