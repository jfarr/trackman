#include "Sample.h"

#include <memory>

#include "Track.h"

Sample::Sample(
    Track &track, juce::File file, double startPos, double endPos, double sourceLengthSecs, double sampleRate)
    : track(&track), file(std::move(file)), startPos(startPos), endPos(endPos), length(sourceLengthSecs),
      sourceLengthSecs(sourceLengthSecs), sourceSampleRate(sampleRate) {}

Sample::~Sample() {}

juce::uint64 Sample::getTotalLength() const { return 0; }

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        offsetSource = std::make_unique<OffsetAudioSource>(*fileSource, startPos, sourceSampleRate);
        resamplingSource =
            std::make_unique<PositionableResamplingAudioSource>(&*offsetSource, false, sampleRate, sourceSampleRate, 2);
    }
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    if (offsetSource != nullptr) {
        offsetSource->setOffsetSeconds(startPos);
    }
//    track->updateLength();
}

void Sample::setLength(double newLength) {
    length = newLength;
    endPos = startPos + newLength;
    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
//    track->updateLength();
}

void Sample::setTrack(Track &newTrack) { track = &newTrack; }
