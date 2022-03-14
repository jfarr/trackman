#pragma once

#include <JuceHeader.h>

#include "SampleThumbnail.h"
#include "model/Track.h"

class TrackLaneControl : public juce::Component {
  public:
    TrackLaneControl(Project &project, Track &track, juce::AudioTransportSource &transport);
    ~TrackLaneControl();

    Track &getTrack() { return track; }
    int getPreferredHeight() const { return preferredHeight; }

    void addThumbnail(SampleThumbnail *thumbnail);
    void clear() { thumbnails.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 75;

    Project &project;
    Track &track;
    juce::AudioTransportSource &transport;

    juce::Label trackLabel;
    std::list<SampleThumbnail *> thumbnails;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneControl)
};