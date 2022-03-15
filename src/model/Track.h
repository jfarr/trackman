#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"
#include "audio/SamplePlayer.h"

class TrackList;

class Track {
  public:
    explicit Track();
    ~Track();

    [[nodiscard]] int getTrackNumber() const { return trackNumber; }
    [[nodiscard]] juce::String getName() const { return name; }
    [[nodiscard]] float getLevelGain() const { return level; }
    [[nodiscard]] bool isMuted() const { return muted; }
    [[nodiscard]] bool isSoloed() const { return soloed; }
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }
    [[nodiscard]] juce::int64 getTotalLengthInSamples() const;

    juce::PositionableAudioSource *getSource() { return meteredSource.get(); }
    foleys::LevelMeterSource *getMeterSource() {
        return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource();
    }

    void setTrackNumber(int newNumber) { trackNumber = newNumber; }
    void setName(juce::String newName) { name = newName; }
    void setLevelGain(float newLevel);
    void setSelected(bool newSelected) { selected = newSelected; }
    Sample *getSelected() const;
    void setDeleted(bool newDeleted);

    void selectSample(Sample *newSelected);
    void moveSampleTo(Sample &sample, Track &toTrack);
    void eachSample(std::function<void(Sample &sample)> f);

  private:
    friend TrackList;

    Sample *addSample(const juce::File &file, double startPos, double endPos, juce::AudioDeviceManager &deviceManager,
        juce::AudioFormatManager &formatManager);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain(bool anySoloed);

    const juce::String defaultName = "untitled";

    int trackNumber = 0;
    juce::String name = defaultName;
    std::unique_ptr<MeteredAudioSource> meteredSource;
    std::unique_ptr<GainAudioSource> gainSource;

    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::shared_ptr<Sample>> samples;
    std::unique_ptr<SamplePlayer> samplePlayer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
