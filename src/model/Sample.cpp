#include "Sample.h"

void Sample::loadFile(juce::AudioFormatManager &formatManager) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        fileSource.reset(new juce::AudioFormatReaderSource(reader, true));
        offsetSource.reset(new OffsetAudioSource(*fileSource, startPos, sampleRate));
    }
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    offsetSource->setOffsetSeconds(startPos);
}
