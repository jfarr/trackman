#include "Sample.h"

#include <memory>

Sample::~Sample() {
    if (offsetSource != nullptr) {
        delete offsetSource;
    }
    if (resamplingSource != nullptr) {
        delete resamplingSource;
    }
}

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource = new juce::AudioFormatReaderSource(reader, true);
        resamplingSource = new PositionableResamplingAudioSource(fileSource, true, sampleRate, sourceSampleRate, 2);
        offsetSource = new OffsetAudioSource(*resamplingSource, startPos, sourceSampleRate);
    }
}

void Sample::setMinLengthSecs(double newLength) {
    if (offsetSource == nullptr) {
        return;
    }
    offsetSource->setMinLength((juce::int64)newLength * (juce::int64)sourceSampleRate);
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    offsetSource->setOffsetSeconds(startPos);
}

void Sample::setLength(double newLength) {
    length = newLength;
    endPos = startPos + newLength;
    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
}
