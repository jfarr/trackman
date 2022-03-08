#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/OffsetAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

class Track {
  public:
    Track(juce::String name);
    ~Track();

    juce::String getName() const { return name; }
    std::shared_ptr<juce::PositionableAudioSource> getSource() const { return source; }
    std::shared_ptr<GainAudioSource> getGain() const { return gain; }
    double getTotalLengthSeconds() const;
    double getSampleRate() const;
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate);
    void adjustSampleLengthSecs(double newLen);
    void loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    Sample *addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, juce::File file,
        double startPos, double endPos, double length, double sampleRate);
    Sample *getSelected() const;
    void eachSample(std::function<void(Sample &sample)> f);

    void setLevelGain(float newLevel);
    void toggleMute();
    void setMute(bool newMuted);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);
    void selectSample(Sample *selected);
    void deleteSample(Sample *sample);
    void undeleteSample(Sample *sample);

  private:
    juce::String name;
    std::shared_ptr<juce::PositionableAudioSource> source = nullptr;
    std::shared_ptr<GainAudioSource> gain;
    std::shared_ptr<OffsetAudioSource> offset;
    std::shared_ptr<PositionableMixingAudioSource> mixer;
    double sampleRate = 0;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::unique_ptr<Sample>> samples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
