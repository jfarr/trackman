#pragma once

#include <JuceHeader.h>

#include "SampleThumbnail.h"
#include "model/Track.h"

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl(Track &track);
    ~TrackLaneControl();

    Track &getTrack() { return track; }

    void addThumbnail(SampleThumbnail *thumbnail);
    void clear() { thumbnails.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Track &track;

    juce::Label trackLabel;
    std::list<SampleThumbnail *> thumbnails;
    float scale = 25;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
