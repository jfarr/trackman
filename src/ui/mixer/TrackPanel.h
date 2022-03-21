#pragma once

#include <JuceHeader.h>

#include "TrackControl.h"

using namespace std;
using namespace juce;

namespace trackman {

class TrackPanel : public Component {
  public:
    TrackPanel(Viewport &viewport);
    ~TrackPanel() override = default;

    void addTrack(TrackControl *lane) { tracks.push_back(lane); }
    void resize();
    void clear() { tracks.clear(); }
    void update();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &event) override;

  private:
    Viewport &viewport;
    list<TrackControl *> tracks;

    int getTrackWidth() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackPanel)
};

} // namespace trackman
