#pragma once

#include "JuceHeader.h"

class TrackListViewport : public juce::Viewport {
  public:
    void visibleAreaChanged (const juce::Rectangle<int>& newVisibleArea) override;
};
