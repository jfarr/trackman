#pragma once

#include <JuceHeader.h>

#include "Sample.h"
#include "audio/GainAudioSource.h"
#include "audio/MeteredAudioSource.h"
#include "audio/OffsetAudioSource.h"
#include "audio/PositionableMixingAudioSource.h"

//class TrackList;

class Track {
  public:
    explicit Track();
    ~Track();

    [[nodiscard]] int getTrackNumber() const { return trackNumber; }
    [[nodiscard]] juce::String getName() const { return name; }
    [[nodiscard]] double getTotalLengthSeconds() const { return 0; }
    [[nodiscard]] float getLevelGain() const { return level; }
    [[nodiscard]] bool isMuted() const { return muted; }
    [[nodiscard]] bool isSoloed() const { return soloed; }
//    [[nodiscard]] bool isSilenced() const;
    [[nodiscard]] bool isSelected() const { return selected; }
    [[nodiscard]] bool isDeleted() const { return deleted; }

//    juce::PositionableAudioSource *getSource() { return meteredSource.get(); }
//    foleys::LevelMeterSource *getMeterSource() {
//        return meteredSource == nullptr ? nullptr : &meteredSource->getMeterSource();
//    }
//
//    void loadSamples(juce::AudioFormatManager &formatManager);
    Sample *addSample(juce::AudioFormatManager &formatManager,
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
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted);

  private:
//    friend TrackList;
//    friend Sample;

//    void setSource(std::shared_ptr<juce::PositionableAudioSource> newSource);

    const juce::String defaultName = "untitled";

//    TrackList &trackList;
//    juce::AudioDeviceManager &deviceManager;
    int trackNumber = 0;
    juce::String name = defaultName;
//    std::shared_ptr<juce::PositionableAudioSource> source = nullptr;
//    std::shared_ptr<MeteredAudioSource> meteredSource;
//    std::shared_ptr<GainAudioSource> gainSource;
//    std::shared_ptr<PositionableMixingAudioSource> mixerSource;

    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool soloed = false;
    bool selected = false;
    bool deleted = false;
    std::list<std::shared_ptr<Sample>> samples;
//    double totalLengthSecs = 0;

//    void updateLength();
//    void setTotalLengthSecs(double newLen);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
