#pragma once

#include "JuceHeader.h"
#include "controls/mixer/TrackLaneControl.h"
#include "model/Track.h"

class TrackListPanel : public juce::Component {
  public:
    TrackListPanel();
    ~TrackListPanel();

    void addTrack(TrackLaneControl& lane);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    std::list<TrackLaneControl *> lanes;
};
