#pragma once

#include <JuceHeader.h>

class MixerPanelListener {
  public:
    virtual void mixerResized(juce::Rectangle<int> area) = 0;
};
