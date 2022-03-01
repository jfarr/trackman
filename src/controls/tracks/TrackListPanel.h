#pragma once

#include "JuceHeader.h"
#include "model/Track.h"

class TrackListPanel : public juce::Component {
  public:
    TrackListPanel();
    ~TrackListPanel();

    void addTrack(Track &track);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    std::list<Track *> tracks;
};
