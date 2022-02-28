#pragma once

#include "audio/ProcessingAudioSource.h"
#include "controls/mixer/TrackControl.h"
#include "controls/mixer/TrackLaneControl.h"

class TrackSourceListener {
  public:
    virtual void onSourceSet(juce::PositionableAudioSource *newSource, juce::PositionableAudioSource *prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) = 0;
};

class GainProcessor : public juce::dsp::ProcessorWrapper<juce::dsp::Gain<float>> {};

class Track : public FileListener, public TrackControlListener {
  public:
    Track(juce::String name, juce::AudioFormatManager &formatManager);
    ~Track();

    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    TrackControl &getTrackControl() { return trackControl; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    //==============================================================================
    // FileListener
    void fileChosen(juce::File file) override;

    //==============================================================================
    // TrackControlListener
    void levelChanged(float level) override;
    void muteChanged(bool muted) override;

  private:
    juce::AudioFormatManager &formatManager;

    // std::unique_ptr<ProcessingAudioSource<juce::dsp::Gain<float>>> source;
    // std::unique_ptr<juce::PositionableAudioSource> source;
    juce::PositionableAudioSource *source;
    TrackSourceListener *listener;

    // GainProcessor gain;
    TrackControl trackControl;
    TrackLaneControl trackLaneControl;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
