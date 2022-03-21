#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

#include "MasterLevelMeterLookAndFeel.h"
#include "MasterTrackListener.h"
#include "model/Mixer.h"
#include "ui/common/DecibelSlider.h"

using namespace std;
using namespace juce;
using namespace foleys;

namespace trackman {

class MasterTrackControl : public Component, public SliderListener {
  public:
    MasterTrackControl(Mixer &mixer, LevelMeterSource &meterSource);
    ~MasterTrackControl();

    static int getPreferredWidth() { return 150; }

    void update();

    void setLevel(float level);

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;
    void mouseUp(const MouseEvent &event) override;

    //==============================================================================
    // SliderListener
    void sliderClicked() override;

  private:
    Mixer &mixer;

    DecibelSlider decibelSlider;
    TextButton muteButton;
    Label channelLabel;
    MasterLevelMeterLookAndFeel levelMeterLookAndFeel;
    LevelMeter levelMeter;

    bool draggingSlider = false;
    float previousLevel;
    list<MasterTrackListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();

    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterTrackControl)
};

} // namespace trackman
