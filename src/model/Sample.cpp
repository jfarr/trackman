#include "Sample.h"

#include <memory>

#include "Track.h"

Sample::~Sample() {
    delete offsetSource;
    delete resamplingSource;
}

void Sample::loadFile(juce::AudioFormatManager &formatManager, double sampleRate) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        delete offsetSource;
        delete resamplingSource;
        fileSource = new juce::AudioFormatReaderSource(reader, true);
        resamplingSource = new PositionableResamplingAudioSource(fileSource, true, sampleRate, sourceSampleRate, 2);
        offsetSource = new OffsetAudioSource(*resamplingSource, startPos, sourceSampleRate);
    }
}

void Sample::setPosition(double pos) {
    startPos = pos;
    endPos = startPos + length;
    offsetSource->setOffsetSeconds(startPos);
    track.updateLength();
}

void Sample::setLength(double newLength) {
    length = newLength;
    endPos = startPos + newLength;
    auto sourceSampleRateToCorrectFor = sourceSampleRate * sourceLengthSecs / newLength;
    resamplingSource->setSourceSampleRateToCorrectFor(sourceSampleRateToCorrectFor);
    track.updateLength();
}
