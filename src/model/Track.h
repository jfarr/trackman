#pragma once

#include <JuceHeader.h>

class Track {
  public:
    Track(juce::String name);
    ~Track();

    juce::String getName() { return name; }
    float getLevel() const { return level; }
    bool getMuted() const { return muted; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setLevel(float newLevel) { level = newLevel; }
    void setMuted(bool newMuted) { muted = newMuted; }
    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

  private:
    juce::String name;
    float level = juce::Decibels::decibelsToGain<float>(0.0);
    bool muted = false;
    bool selected = false;
    bool deleted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
