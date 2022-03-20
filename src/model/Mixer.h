#pragma once

#include <JuceHeader.h>

#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

class TrackList;

class Mixer : public juce::PositionableAudioSource {
  public:
    Mixer(TrackList &trackList, juce::AudioDeviceManager &deviceManager);
    ~Mixer() override;

    float getMasterLevelGain() const { return level; }
    bool isMasterMuted() const { return muted; }

    juce::AudioTransportSource &getTransportSource() { return transportSource; }
    juce::AudioSource &getSource() { return *((PositionableMixingAudioSource *)&mixerSource); }
    foleys::LevelMeterSource &getMeterSource() { return meteredSource.getMeterSource(); }

    void addSource(juce::PositionableAudioSource *source);
    void removeSource(juce::PositionableAudioSource *source);
    void removeAllSources();

    void setMasterLevelGain(float newLevel);
    void setMasterMute(bool newMuted);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(juce::int64 newPosition) override { meteredSource.setNextReadPosition(newPosition); }
    juce::int64 getNextReadPosition() const override { return meteredSource.getNextReadPosition(); }
    juce::int64 getTotalLength() const override { return meteredSource.getTotalLength(); }
    bool isLooping() const override { return meteredSource.isLooping(); }
    void setLooping(bool shouldLoop) override { meteredSource.setLooping(shouldLoop); }

  private:
    TrackList &trackList;
    juce::AudioDeviceManager &deviceManager;
    std::list<juce::PositionableAudioSource *> sources;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    MeteredAudioSource meteredSource;
    juce::AudioTransportSource transportSource;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
