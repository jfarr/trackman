#pragma once

#include "JuceHeader.h"
#include "TrackLaneControl.h"
#include "model/Track.h"
#include "model/TrackList.h"

class TrackListPanel : public juce::Component {
  public:
    TrackListPanel(TrackList &trackList, juce::Viewport &viewport);
    ~TrackListPanel();

    void addLane(TrackLaneControl* lane) { lanes.push_back(lane); }
    void resize();
    void clear() { lanes.clear(); }

    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    TrackList &trackList;
    juce::Viewport &viewport;
    std::list<TrackLaneControl *> lanes;
    float scale = 25;

    int getTrackLaneWidth() const;
};
