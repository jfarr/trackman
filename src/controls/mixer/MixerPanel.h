#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "controls/common/TransportControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class DesktopController;

class MixerPanel : public juce::Component {
  public:
    MixerPanel(DesktopController &desktopController, TrackList &trackList, Mixer &mixer, foleys::LevelMeterSource &meterSource);
    ~MixerPanel();

    int getPreferredHeight() const { return std::max(getHeight(), 280); }

    void clear();
    void update();
    void addTrack(TrackControl *trackControl);

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    DesktopController &desktopController;
    TrackList &trackList;
    Mixer &mixer;

    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<TrackControl *> tracks;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
