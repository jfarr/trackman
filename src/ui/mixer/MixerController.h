#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "TrackControl.h"
#include "TrackController.h"
#include "TrackPanel.h"
#include "model/Mixer.h"
#include "model/TrackList.h"
#include "ui/mixer/MasterTrackListener.h"

using namespace std;

namespace trackman {

class DesktopController;

class MixerController : public MasterTrackListener {
  public:
    MixerController(DesktopController &desktopController);
    ~MixerController();

    bool isMasterMuted() const { return mixer.isMasterMuted(); }

    MixerPanel &getMixerPanel() { return mixerPanel; }
    juce::Viewport &getViewport() { return mixerViewport; }

    void update();
    void repaint();

    void setMasterLevel(float newLevel);
    void setMasterMute(bool newMute);
    void setLevel(Track &track, float newLevel);
    void setMute(Track &track, bool newMute);
    void setSolo(Track &track, bool newSolo);

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;

  private:
    DesktopController &desktopController;
    TrackList &trackList;
    Mixer &mixer;

    juce::Viewport mixerViewport;
    MixerPanel mixerPanel;
    TrackPanel trackPanel;
    list<std::unique_ptr<TrackController>> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};

} // namespace trackman
