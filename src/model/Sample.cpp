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

juce::int64 Sample::getLengthInSamples() const {
    return resamplingSource == nullptr ? 0 : resamplingSource->getTotalLength();
}

void Sample::loadFile(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager) {
    auto blockSize = deviceManager.getAudioDeviceSetup().bufferSize;
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    reader.reset(formatManager.createReaderFor(file));
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader.get(), false);
        //        offsetSource = std::make_unique<OffsetAudioSource>(*fileSource, startPos, sourceSampleRate);
        sourceSampleRate = reader->sampleRate;
        resamplingSource = std::make_unique<PositionableResamplingAudioSource>(
            fileSource.get(), false, sampleRate, sourceSampleRate, 2);
        resamplingSource->prepareToPlay(blockSize, sampleRate);
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
    //    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    //    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
    //    track->updateLength();
}

// void Sample::setTrack(Track &newTrack) { track = &newTrack; }
