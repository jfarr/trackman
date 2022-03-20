#pragma once

#include <JuceHeader.h>

#include "model/Project.h"

class TimeMeter : public juce::Component {
  public:
    TimeMeter(Project &project) : project(project) {}
    ~TimeMeter() {}

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;

  private:
    Project &project;

    void drawTicksInSeconds(juce::Graphics &g, const juce::Rectangle<int> &bounds) const;
    void drawTicksInMeasures(juce::Graphics &g, const juce::Rectangle<int> &bounds) const;
    void drawStartMarker(juce::Graphics &g) const;
};
