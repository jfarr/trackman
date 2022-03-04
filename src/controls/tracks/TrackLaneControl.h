#pragma once

#include <JuceHeader.h>

#include "model/Track.h"

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl(Track &track);
    ~TrackLaneControl();

    void setSelected(bool newSelected);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Track &track;
//    bool selected = false;

    juce::Label trackLabel;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
