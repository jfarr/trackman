#pragma once

#include <JuceHeader.h>

#include "audio/PositionableMixingAudioSource.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"

class Mixer {
  public:
    Mixer(double sampleRate);
    ~Mixer();

    float getMasterLevelGain() const { return level; }
    bool isMasterMuted() const { return muted; }

    juce::AudioTransportSource &getTransportSource() { return transportSource; }
    juce::AudioSource &getSource() { return *((PositionableMixingAudioSource *) &mixerSource); }
    foleys::LevelMeterSource &getMeterSource() { return meteredSource.getMeterSource(); }

    void addSource(std::shared_ptr<juce::PositionableAudioSource> source, double sourceSampleRateToCorrectFor = 0.0,
        int maxNumChannels = 2);
    void removeAllSources();

    void setMasterLevelGain(float newLevel);
    void setMasterMute(bool newMuted);
    void setLooping(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
    void releaseResources();

  private:
    std::list<std::shared_ptr<juce::PositionableAudioSource>> sources;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    MeteredAudioSource meteredSource;
    juce::AudioTransportSource transportSource;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool initialized = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
