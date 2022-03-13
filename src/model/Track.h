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
    explicit Track(TrackList& trackList);
    ~Track() = default;

    [[nodiscard]] int getTrackNumber() const { return trackNumber; }
    [[nodiscard]] juce::String getName() const { return name; }
    [[nodiscard]] juce::uint64 getTotalLength() const;
    [[nodiscard]] double getTotalLengthSeconds() const;
    [[nodiscard]] double getSampleRate() const;
    [[nodiscard]] float getLevelGain() const { return level; }
    [[nodiscard]] bool isMuted() const { return muted; }
    [[nodiscard]] bool isSoloed() const { return soloed; }
    [[nodiscard]] bool isSilenced() const;
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

    juce::PositionableAudioSource &getSource() { return meteredSource; }
    foleys::LevelMeterSource *getMeterSource() { return &meteredSource.getMeterSource(); }

    void adjustSampleLengthSecs(double newLen);
    void loadSamples(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    Sample *addSample(juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, const juce::File& file,
        double startPos, double endPos, double length, double sampleRate);
    void moveSampleTo(Sample &sample, Track &toTrack);
    Sample *getSelected() const;
    void eachSample(std::function<void(Sample &sample)> f);

    void setTrackNumber(int newNumber) { trackNumber = newNumber; }
    void setName(juce::String newName) { name = newName; }
    void setLevelGain(float newLevel);
    void setMute(bool newMuted);
    void setSolo(bool newSoloed);
    void updateGain();
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);
    void selectSample(Sample *newSelected);
    void deleteSample(Sample *sample);
    void undeleteSample(Sample *sample);

  private:
    const juce::String defaultName = "untitled";

    TrackList& trackList;
    int trackNumber = 0;
    juce::String name = defaultName;
    PositionableMixingAudioSource mixerSource;
    GainAudioSource gainSource;
    MeteredAudioSource meteredSource;

//    double sampleRate = 0;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::shared_ptr<Sample>> samples;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
