#pragma once

#include <JuceHeader.h>

#include "audio/GainAudioSource.h"

class Track {
  public:
    Track(juce::String name);
    ~Track();

    juce::String getName() const { return name; }
    juce::File getFile() const { return file; }
    std::shared_ptr<juce::PositionableAudioSource> getSource() const { return source; }
    std::shared_ptr<GainAudioSource> getGain() const { return gain; }
    double getSampleRate() const { return sampleRate; }
    float getLevelGain() const { return level; }
    bool getMuted() const { return muted; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setFile(juce::File newFile) { file = newFile; }
    void setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate);
    void setLevel(float newLevel);
    void setMuted(bool newMuted);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    juce::String name;
    juce::File file;
    std::shared_ptr<juce::PositionableAudioSource> source = nullptr;
    std::shared_ptr<GainAudioSource> gain;
    double sampleRate = 0;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool selected = false;
    bool deleted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
