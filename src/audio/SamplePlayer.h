#pragma once

#include <JuceHeader.h>

#include "TimeRange.h"
#include "Timeline.h"
#include "model/Sample.h"

using namespace std;
using namespace juce;

namespace trackman {

class SamplePlayer : public PositionableAudioSource {
  public:
    SamplePlayer(list<shared_ptr<Sample>> &samples);
    ~SamplePlayer() override;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    list<shared_ptr<Sample>> &samples;
    double currentSampleRate = 0;
    int64 currentPos = 0;
    bool looping = false;
    AudioBuffer<float> tempBuffer;

    Timeline<Sample *> getCurrentTimeline();
    double getTimeAtPosition(int64 position) { return position / currentSampleRate; }

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplePlayer)
};

} // namespace trackman
