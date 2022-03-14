#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/OffsetAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

class TrackList;

class Track {
  public:
    explicit Track(TrackList &trackList, juce::AudioDeviceManager &deviceManager);
    ~Track() { samples.clear(); }

    [[nodiscard]] int getTrackNumber() const { return trackNumber; }
    [[nodiscard]] juce::String getName() const { return name; }
//    [[nodiscard]] juce::uint64 getTotalLength() const;
    [[nodiscard]] double getTotalLengthSeconds() const;
//    [[nodiscard]] double getSampleRate() const;
    [[nodiscard]] float getLevelGain() const { return level; }
    [[nodiscard]] bool isMuted() const { return muted; }
    [[nodiscard]] bool isSoloed() const { return soloed; }
    [[nodiscard]] bool isSilenced() const;
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    juce::PositionableAudioSource *getSource() { return meteredSource.get(); }
    foleys::LevelMeterSource *getMeterSource() {
        return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource();
    }

//    void adjustSampleLengthSecs(double newLen);
    void loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    Sample *addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager,
        const juce::File &file, double startPos, double endPos, double length, double sampleRate);
    void moveSampleTo(Sample &sample, Track &toTrack);
    void deleteSample(Sample *sample);
    void undeleteSample(Sample *sample);

    void setTrackNumber(int newNumber) { trackNumber = newNumber; }
    void setName(juce::String newName) { name = newName; }
    void setLevelGain(float newLevel);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain();
    Sample *getSelected() const;
    void selectSample(Sample *newSelected);
    void eachSample(std::function<void(Sample &sample)> f);

  private:
    friend TrackList;
    friend Sample;

    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);

    const juce::String defaultName = "untitled";

    TrackList &trackList;
    int trackNumber = 0;
    juce::String name = defaultName;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    std::unique_ptr<MeteredAudioSource> meteredSource;

    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::shared_ptr<Sample>> samples;
    double totalLengthSecs = 0;

    void updateLength();
    void setTotalLengthSecs(double newLen);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
