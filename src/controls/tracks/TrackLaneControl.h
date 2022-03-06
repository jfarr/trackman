#pragma once

#include <JuceHeader.h>

#include "SampleThumbnail.h"
#include "model/Track.h"

class TrackLaneControl : public juce::Component, private juce::Timer {
  public:
    TrackLaneControl(Track &track, juce::AudioTransportSource &transport);
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
    juce::AudioTransportSource &transport;

    juce::Label trackLabel;
    std::list<SampleThumbnail *> thumbnails;
    float scale = 75;

    void createControls();
    void timerCallback() override { repaint(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};
