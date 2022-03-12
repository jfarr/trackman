#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/OffsetAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"
#include "audio/MeteredAudioSource.h"

class TrackList;

class Track {
  public:
    Track(TrackList& trackList);
    ~Track();

    int getNumber() const { return number; }
    juce::String getName() const { return name; }
    std::shared_ptr<juce::PositionableAudioSource> getSource() const { return meteredSource; }
    juce::uint64 getTotalLength() const;
    double getTotalLengthSeconds() const;
    double getSampleRate() const;
    float getLevelGain() const { return level; }
    bool isMuted() const { return muted; }
    bool isSoloed() const { return soloed; }
    bool isSilenced() const;
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }
    foleys::LevelMeterSource *getMeterSource() { return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource(); }

    void setSource(std::shared_ptr<juce::PositionableAudioSource> newSource, double newSampleRate);
    void adjustSampleLengthSecs(double newLen);
    void loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    Sample *addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, juce::File file,
        double startPos, double endPos, double length, double sampleRate);
    void moveSampleTo(Sample &sample, Track &toTrack);
    Sample *getSelected() const;
    void eachSample(std::function<void(Sample &sample)> f);

    void setNumber(int newNumber) { number = newNumber; }
    void setName(juce::String newName) { name = newName; }
    void setLevelGain(float newLevel);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain();
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);
    void selectSample(Sample *selected);
    void deleteSample(Sample *sample);
    void undeleteSample(Sample *sample);

  private:
    const juce::String defaultName = "untitled";

    TrackList& trackList;
    int number = 0;
    juce::String name = defaultName;
    std::shared_ptr<juce::PositionableAudioSource> source = nullptr;
    std::shared_ptr<MeteredAudioSource> meteredSource;
    std::shared_ptr<GainAudioSource> gainSource;
    std::shared_ptr<OffsetAudioSource> offsetSource;
    std::shared_ptr<PositionableMixingAudioSource> mixerSource;
    double sampleRate = 0;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::shared_ptr<Sample>> samples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
