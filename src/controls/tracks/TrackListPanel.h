#pragma once

#include "JuceHeader.h"
#include "TrackLaneControl.h"
#include "model/Track.h"
#include "model/TrackList.h"

class TrackListPanel : public juce::Component {
  public:
    TrackListPanel(TrackList &trackList);
    ~TrackListPanel();

    void update();
//
//    void addTrack(TrackLaneControl &lane);
//    void removeTrack(TrackLaneControl &lane);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    TrackList &trackList;
    std::list<std::unique_ptr<TrackLaneControl>> lanes;
};
