#pragma once

#include <JuceHeader.h>

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl();
    ~TrackLaneControl();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
