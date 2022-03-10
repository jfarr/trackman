#include "Sample.h"

void Sample::loadFile(juce::AudioFormatManager &formatManager) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource.reset(new juce::AudioFormatReaderSource(reader, true));
        offsetSource.reset(new OffsetAudioSource(*fileSource, startPos, sourceSampleRate));
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
    endPos = startPos + sourceLengthSecs;
    offsetSource->setOffsetSeconds(startPos);
}

void Sample::setLength(double length) {
}
