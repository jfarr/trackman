#include "Sample.h"

#include <memory>

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        resamplingSource =
            std::make_unique<PositionableResamplingAudioSource>(&*fileSource, false, sampleRate, sourceSampleRate, 2);
        offsetSource = std::make_unique<OffsetAudioSource>(*resamplingSource, startPos, sourceSampleRate);
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
