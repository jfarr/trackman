#include "Sample.h"

#include <memory>

#include "Track.h"

Sample::~Sample() {
}

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        resamplingSource = std::make_unique<PositionableResamplingAudioSource>(&*fileSource, false, sampleRate, sourceSampleRate, 2);
        offsetSource = std::make_unique<OffsetAudioSource>(*resamplingSource, startPos, sourceSampleRate);
    }
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    offsetSource->setOffsetSeconds(startPos);
    track->updateLength();
}

void Sample::setLength(double newLength) {
    length = newLength;
    endPos = startPos + newLength;
    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
    track->updateLength();
}

void Sample::setTrack(Track &newTrack) {
    track = &newTrack;
}
