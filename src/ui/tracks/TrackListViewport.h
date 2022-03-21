#pragma once

#include <JuceHeader.h>
#include "model/Project.h"

using namespace juce;

namespace trackman {

class DesktopComponent;

class TrackListViewport : public Viewport, private Timer {

  public:
    TrackListViewport(DesktopComponent &desktop, Project &project) : desktop(desktop), project(project) {
        startTimer(20);
    }
    ~TrackListViewport() override = default;

    void visibleAreaChanged(const Rectangle<int> &newVisibleArea) override;

  private:
    void timerCallback() override;

    DesktopComponent &desktop;
    Project &project;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListViewport)
};

}
