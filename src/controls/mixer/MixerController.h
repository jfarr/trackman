#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "TrackControl.h"
#include "TrackController.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/mixer/MasterTrackListener.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class DesktopController;

class MixerController : public TransportControlListener, public MasterTrackListener {
  public:
    MixerController(DesktopController &desktopController, juce::AudioFormatManager &formatManager);
    ~MixerController();

    bool isMasterMuted() const { return mixer.isMasterMuted(); }

    MixerPanel &getMixerPanel() { return mixerPanel; }
    Mixer &getMixer() { return mixer; }

    void update();
    void repaint();

    void updateAudioSource();
    void setMasterLevel(float newLevel);
    void setMasterMute(bool newMute);
    void setLevel(Track &track, float newLevel);
    void setMute(Track &track, bool newMute);
    void setSolo(Track &track, bool newSolo);

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;

  private:
    DesktopController &desktopController;
    TrackList &trackList;
    Mixer &mixer;
    juce::AudioFormatManager &formatManager;

    MixerPanel mixerPanel;
    std::list<std::unique_ptr<TrackController>> tracks;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
