#pragma once

#include "JuceHeader.h"

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl();
    ~TrackLaneControl();

    void setSelected(bool newSelected);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    bool selected = false;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
