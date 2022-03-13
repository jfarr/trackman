#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"
#include "ui/common/TransportControl.h"

class DesktopController;

class MixerPanel : public juce::Component {
  public:
    MixerPanel(DesktopController &desktopController,
        /*TrackList &trackList, Mixer &mixer,*/ foleys::LevelMeterSource &meterSource);
    ~MixerPanel() override;

    int getPreferredHeight() const { return std::max(getHeight(), 280); }

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    DesktopController &desktopController;
    juce::Viewport &mixerViewport;

    TransportControl transportControl;
    MasterTrackControl masterTrackControl;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
