#pragma once

#include "JuceHeader.h"

class Track {
  public:
    Track(juce::String name);
    ~Track();

    juce::String getName() { return name; }

  private:
    juce::String name;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Track)
};
