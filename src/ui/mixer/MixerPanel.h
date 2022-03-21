#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MasterTrackListener.h"
#include "TrackControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

using namespace std;
using namespace juce;
using namespace foleys;

namespace trackman {

class DesktopController;

class MixerPanel : public Component {
  public:
    MixerPanel(DesktopController &desktopController, LevelMeterSource &meterSource);
    ~MixerPanel() override;

    int getPreferredHeight() const { return max(getHeight(), 280); }

    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    void update();

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

  private:
    const int preferredWidth = 800;
    const int preferredHeight = 280;

    DesktopController &desktopController;
    Viewport &mixerViewport;

    MasterTrackControl masterTrackControl;

    list<MasterTrackListener *> listeners;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};

} // namespace trackman