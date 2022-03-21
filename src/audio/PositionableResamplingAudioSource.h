#pragma once

#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class PositionableResamplingAudioSource : public PositionableAudioSource {
  public:
    PositionableResamplingAudioSource(PositionableAudioSource *source, const bool deleteWhenRemoved, double sampleRate,
        double sourceSampleRateToCorrectFor, int maxNumChannels = 2);
    ~PositionableResamplingAudioSource() override;

    PositionableAudioSource *getSource() { return source; }
    double getSampleRate() const { return sampleRate; }

    void setSourceSampleRateToCorrectFor(double newSampleRate);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &info) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    PositionableAudioSource *source;
    ResamplingAudioSource resamplerSource;
    double sampleRate, sourceSampleRate;
    int blockSize = 128;
    const bool deleteWhenRemoved;

    double getSampleRatio() const {
        return (sampleRate > 0 && sourceSampleRate > 0) ? sampleRate / sourceSampleRate : 1.0;
    }

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionableResamplingAudioSource)
};

} // namespace trackman
