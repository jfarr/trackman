#pragma once

#include "TimeRange.h"
#include "Timeline.h"
#include "model/Sample.h"
#include <JuceHeader.h>

class SamplePlayer : public juce::PositionableAudioSource {
  public:
    SamplePlayer(std::list<std::shared_ptr<Sample>> &samples) : samples(samples) {}
    ~SamplePlayer() override = default;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override;
    juce::int64 getNextReadPosition() const override;
    juce::int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    std::list<std::shared_ptr<Sample>> &samples;
//    int currentBlockSize = 0;
    double currentSampleRate = 0;
    juce::int64 currentPos = 0;
    bool looping = false;
    Timeline<Sample *> timeline;
    juce::AudioBuffer<float> tempBuffer;

    void updateTimeline();
    double getTimeAtPosition(juce::int64 position) { return position / currentSampleRate; }

    juce::CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplePlayer)
};
