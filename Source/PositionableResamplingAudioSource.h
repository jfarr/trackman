#pragma once

#include <JuceHeader.h>


class PositionableResamplingAudioSource : public juce::PositionableAudioSource
{
public:
	PositionableResamplingAudioSource(juce::PositionableAudioSource* source, const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor, int maxNumChannels = 2);
	~PositionableResamplingAudioSource();

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& info) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

private:
	juce::PositionableAudioSource* source;
    juce::ResamplingAudioSource resamplerSource;
    //juce::CriticalSection callbackLock;
    double sampleRate = 44100.0, sourceSampleRate;
    int blockSize = 128;
    const bool deleteWhenRemoved;

    double getSampleRatio() const { return (sampleRate > 0 && sourceSampleRate > 0) ? sampleRate / sourceSampleRate : 1.0; }
};
