#pragma once

#include <JuceHeader.h>
#include <utility>

#include "TrackRegion.h"
#include "audio/PositionableResamplingAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Track;

class Sample : public TrackRegion, public PositionableAudioSource {
  public:
    Sample(File file, double startPosInSeconds, double endPosInSeconds);
    ~Sample() override;

    File getFile() const { return file; }
    double getStartPosInSeconds() const override { return startPosInSeconds; }
    double getEndPosInSeconds() const override { return endPosInSeconds; }
    double getLengthInSeconds() const { return lengthInSeconds; }
    int64 getTotalLengthInSamples() const;
    bool isLoaded() const { return loaded; }

    void loadFile(AudioDeviceManager &deviceManager, AudioFormatManager &formatManager);

    void setPosition(double newPosInSeconds);
    void setLength(double newLengthInSeconds);

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
    File file;
    double startPosInSeconds;
    double endPosInSeconds;
    double lengthInSeconds;
    double sourceLengthInSeconds = 0;
    unique_ptr<AudioFormatReaderSource> fileSource;
    unique_ptr<PositionableResamplingAudioSource> resamplingSource;
    bool loaded = false;

    int64 getPositionFromTime(double t) const;
    double getSampleRate() const;

    unique_ptr<AudioFormatReader> reader;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Sample)
};

} // namespace trackman
