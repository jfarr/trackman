#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "TrackControl.h"
#include "TrackControlListener.h"
#include "TrackController.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/mixer/MasterTrackListener.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class DesktopController;

class MixerController : public TrackListListener,
                        public TransportControlListener,
                        public MasterTrackListener,
                        public TrackControlListener {
  public:
    MixerController(
        DesktopController &desktop, TrackList &trackList, Mixer &mixer, juce::AudioFormatManager &formatManager);
    ~MixerController();

    MixerPanel &getMixerPanel() { return mixerPanel; }
    Mixer &getMixer() { return mixer; }

    void update();
    void repaint();

    void updateAudioSource();
    void setMasterLevel(float newLevel);
    void toggleMasterMute();
    void setLevel(Track &track, float newLevel);
    void toggleMute(Track &track);
    void toggleSolo(Track &track);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    void addListener(TrackControlListener *listener);
    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;
    void masterLevelChangeFinalized(float previousLevel) override;
    void masterMuteToggled() override;

    //==============================================================================
    // TrackControlListener
    void nameChanged(Track &track, juce::String newName) override;
    void levelChangeFinalized(Track &track, float previousLevel) override;
    void soloToggled(Track &track) override;
    void muteToggled(Track &track) override;

  private:
    DesktopController &desktop;
    TrackList &trackList;
    Mixer &mixer;
    MixerPanel mixerPanel;
    std::list<std::unique_ptr<TrackController>> tracks;
    juce::AudioFormatManager &formatManager;
    std::list<TrackListListener *> trackListListeners;
    std::list<MasterTrackListener *> masterTrackListeners;
    std::list<TrackControlListener *> trackControlListeners;

    //    void notifySelectionChanged(Track *track);
    void notifyMasterLevelChangeFinalized(float previousLevel);
    void notifyMasterMuteToggled();
    void notifyNameChanged(Track &track, juce::String newName);
    void notifyLevelChangeFinalized(Track &track, float previousLevel);
    void notifyMuteToggled(Track &track);
    void notifySoloToggled(Track &track);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
