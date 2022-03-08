#pragma once

#include <JuceHeader.h>

#include "audio/PositionableMixingAudioSource.h"
#include "controls/desktop/TrackSourceListener.h"
#include "audio/MixerAudioSource.h"

class Mixer {
  public:
    Mixer(double sampleRate);
    ~Mixer();

    float getMasterLevelGain() const { return level; }
    bool isMasterMuted() const { return muted; }

    juce::AudioTransportSource &getTransportSource() { return transportSource; }
    foleys::LevelMeterSource &getMeterSource() { return mixerSource.getMeterSource(); }

    void addSource(std::shared_ptr<juce::PositionableAudioSource> source, double sourceSampleRateToCorrectFor = 0.0,
        int maxNumChannels = 2);
    void removeAllSources();

    void setMasterLevelGain(float newLevel);
    void toggleMasterMute();
    void setMasterMute(bool newMuted);
    void setLooping(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
    void releaseResources();

  private:
    std::list<std::shared_ptr<juce::PositionableAudioSource>> sources;
    MixerAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};
