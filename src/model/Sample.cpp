#include "Sample.h"

#include <memory>

#include "Track.h"

Sample::Sample(juce::File file, double startPos, double endPos)
    : file(std::move(file)), startPos(startPos), endPos(endPos), length(endPos - startPos) {}

Sample::~Sample() {}

juce::uint64 Sample::getTotalLength() const { return reader == nullptr ? 0 : reader->lengthInSamples; }

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    reader.reset(formatManager.createReaderFor(file));
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader.get(), true);
        //        offsetSource = std::make_unique<OffsetAudioSource>(*fileSource, startPos, sourceSampleRate);
        //        resamplingSource =
        //            std::make_unique<PositionableResamplingAudioSource>(&*offsetSource, false, sampleRate,
        //            sourceSampleRate, 2);
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
