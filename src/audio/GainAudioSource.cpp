#include "GainAudioSource.h"

GainAudioSource::GainAudioSource(PositionableAudioSource *source, const bool deleteWhenRemoved)
    : PositionableAudioSourceAdapter(source, deleteWhenRemoved) {}

GainAudioSource::~GainAudioSource() {}

void GainAudioSource::prepareToPlay(int blockSize, double sampleRate) { source->prepareToPlay(blockSize, sampleRate); }

void GainAudioSource::releaseResources() { source->releaseResources(); }

void GainAudioSource::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    const juce::ScopedLock sl(callbackLock);
    source->getNextAudioBlock(bufferToFill);
    for (int i = bufferToFill.buffer->getNumChannels(); --i >= 0;)
        bufferToFill.buffer->applyGainRamp(i, bufferToFill.startSample, bufferToFill.numSamples, lastGain, gain);
    lastGain = gain;
    DBG("gain source length: " << source->getTotalLength());
}
