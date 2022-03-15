#pragma once

#include <JuceHeader.h>

#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

class Mixer {
  public:
    Mixer(juce::AudioDeviceManager &deviceManager);
    ~Mixer();

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
    void setLooping(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
    void releaseResources();

  private:
    juce::AudioDeviceManager &deviceManager;
    std::list<juce::PositionableAudioSource *> sources;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    MeteredAudioSource meteredSource;
    juce::AudioTransportSource transportSource;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool initialized = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
