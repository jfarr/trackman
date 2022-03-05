#pragma once

#include <JuceHeader.h>

#include "model/Track.h"

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl(Track &track);
    ~TrackLaneControl();

    Track &getTrack() { return track; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Track &track;

    juce::Label trackLabel;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
