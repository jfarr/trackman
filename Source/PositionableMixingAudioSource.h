#pragma once

#include <JuceHeader.h>


class PositionableMixingAudioSource : public juce::PositionableAudioSource
{
public:
    PositionableMixingAudioSource();
    ~PositionableMixingAudioSource();

    void addInputSource(PositionableAudioSource* newInput, bool deleteWhenRemoved);
    void removeInputSource(PositionableAudioSource* input);
    void removeAllInputs();

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
    juce::MixerAudioSource mixer;
    juce::Array<PositionableAudioSource*> inputs;
    juce::int64 position;
    juce::int64 length;
    bool looping;

    void updateLength();
};
