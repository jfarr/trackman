#pragma once

#include <JuceHeader.h>

class TrackListViewport : public juce::Viewport {
  public:
    TrackListViewport(){};
    ~TrackListViewport() {}

    void visibleAreaChanged (const juce::Rectangle<int>& newVisibleArea) override;
    void resized() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListViewport)
};
