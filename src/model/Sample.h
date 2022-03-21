#pragma once

#include <JuceHeader.h>
#include <utility>

#include "audio/PositionableResamplingAudioSource.h"

using namespace std;
using namespace juce;

namespace trackman {

class Track;

class Sample : public PositionableAudioSource {
  public:
    Sample(File file, double startPos, double endPos);
    ~Sample() override;

    File getFile() const { return file; }
    double getStartPos() const { return startPos; }
    double getEndPos() const { return endPos; }
    int64 getLengthInSamples() const;
    double getLengthInSeconds() const { return length; }
    bool isLoaded() const { return loaded; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void loadFile(AudioDeviceManager &deviceManager, AudioFormatManager &formatManager);

    void setPosition(double pos);
    void setLength(double newLength);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

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
    double startPos;
    double endPos;
    double length;
    double sourceLengthInSeconds = 0;
    unique_ptr<AudioFormatReaderSource> fileSource;
    unique_ptr<PositionableResamplingAudioSource> resamplingSource;
    bool loaded = false;
    bool selected = false;
    bool deleted = false;

    int64 getPositionFromTime(double t) const;
    double getSampleRate() const;

    unique_ptr<AudioFormatReader> reader;
};

}
