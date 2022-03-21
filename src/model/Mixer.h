#pragma once

#include <JuceHeader.h>

#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

using namespace std;
using namespace juce;
using namespace foleys;

namespace trackman {

class TrackList;

class Mixer : public PositionableAudioSource {
  public:
    Mixer(TrackList &trackList, AudioDeviceManager &deviceManager);
    ~Mixer() override;

    float getMasterLevelGain() const { return level; }
    bool isMasterMuted() const { return muted; }

    LevelMeterSource &getMeterSource() { return meteredSource.getMeterSource(); }

    void addSource(PositionableAudioSource *source);
    void removeSource(PositionableAudioSource *source);
    void removeAllSources();

    void setMasterLevelGain(float newLevel);
    void setMasterMute(bool newMuted);

    void writeAudioFile(const File &file, int64 lengthInSamples);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override { meteredSource.setNextReadPosition(newPosition); }
    int64 getNextReadPosition() const override { return meteredSource.getNextReadPosition(); }
    int64 getTotalLength() const override { return meteredSource.getTotalLength(); }
    bool isLooping() const override { return meteredSource.isLooping(); }
    void setLooping(bool shouldLoop) override { meteredSource.setLooping(shouldLoop); }

  private:
    TrackList &trackList;
    AudioDeviceManager &deviceManager;
    list<PositionableAudioSource *> sources;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    MeteredAudioSource meteredSource;
    float level = Decibels::decibelsToGain<float>(0.0);
    bool muted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};

} // namespace trackman
