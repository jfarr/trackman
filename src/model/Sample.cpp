#include "Sample.h"

void Sample::loadFile(juce::AudioFormatManager &formatManager) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource.reset(new juce::AudioFormatReaderSource(reader, true));
        resamplingSource.reset(new PositionableResamplingAudioSource(&*fileSource, false, sourceSampleRate, 0, 2));
        offsetSource.reset(new OffsetAudioSource(*resamplingSource, startPos, sourceSampleRate));
    }
}

void Sample::setMinLengthSecs(double newLength) {
    if (offsetSource == nullptr) {
        return;
    }
    offsetSource->setMinLength(newLength * sourceSampleRate);
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
