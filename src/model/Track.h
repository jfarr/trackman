#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"

class Track {
  public:
    Track(juce::String name);
    ~Track();

    juce::String getName() const { return name; }
    juce::File getFile() const { return file; }
    std::shared_ptr<juce::PositionableAudioSource> getSource() const { return source; }
    std::shared_ptr<GainAudioSource> getGain() const { return gain; }
    juce::int64 getTotalLength() const { return source == nullptr ? 0 : source->getTotalLength(); }
    double getSampleRate() const { return sampleRate; }
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setFile(juce::File newFile) { file = newFile; }
    void setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate);
    void loadFile(juce::AudioFormatManager &formatManager, juce::File newFile);
    void setLevelGain(float newLevel);
    void toggleMute();
    void setMute(bool newMuted);
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);

    Sample *addSample(juce::File file, double startPos, double endPos, double length, double sampleRate);
    void eachSample(std::function<void(Sample &sample)> f);

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
    std::list<std::unique_ptr<Sample>> samples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
