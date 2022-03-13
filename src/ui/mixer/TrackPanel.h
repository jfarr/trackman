#pragma once

#include <JuceHeader.h>

#include "TrackControl.h"

class TrackPanel : public juce::Component {
  public:
    TrackPanel(juce::Viewport &viewport);
    ~TrackPanel() override = default;

    void addTrack(TrackControl *lane) { tracks.push_back(lane); }
    void resize();
    void clear() { tracks.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    juce::Viewport &viewport;
    std::list<TrackControl *> tracks;

    int getTrackWidth() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanel)
};
