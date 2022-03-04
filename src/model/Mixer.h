#pragma once

#include <JuceHeader.h>

#include "audio/PositionableMixingAudioSource.h"
#include "controls/desktop/TrackSourceListener.h"

class Mixer {
  public:
    Mixer();
    ~Mixer();

    juce::AudioTransportSource &getTransportSource() { return transportSource; }

    void addSource(std::shared_ptr<juce::PositionableAudioSource> source, const bool deleteWhenRemoved,
        double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2);
    void removeSource(std::shared_ptr<juce::PositionableAudioSource> source);

    void setMasterLevel(float newLevel);
    void toggleMasterMute();
    void setLooping(bool shouldLoop) { mixerSource.setLooping(shouldLoop); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill);
    void releaseResources();

  private:
    std::list<std::shared_ptr<juce::PositionableAudioSource>> sources;
    PositionableMixingAudioSource mixerSource;
    juce::AudioTransportSource transportSource;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Mixer)
};