#pragma once

#include <JuceHeader.h>

class DesktopComponent;

class TrackListViewport : public juce::Viewport {

  public:
    TrackListViewport(DesktopComponent &desktop) : desktop(desktop) {}
    ~TrackListViewport() = default;

    void visibleAreaChanged (const juce::Rectangle<int>& newVisibleArea) override;

  private:
    DesktopComponent &desktop;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListViewport)
};

