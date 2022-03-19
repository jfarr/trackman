#pragma once

#include <JuceHeader.h>
#include "model/Project.h"

class DesktopComponent;

class TrackListViewport : public juce::Viewport, private juce::Timer {

  public:
    TrackListViewport(DesktopComponent &desktop, Project &project)
        : desktop(desktop), project(project) { startTimer(20); }
    ~TrackListViewport() override = default;

    void visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) override;

  private:
    void timerCallback() override;

    DesktopComponent &desktop;
    Project &project;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListViewport)
};
