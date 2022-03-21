#pragma once

#include <JuceHeader.h>

#include "LevelMeterLookAndFeel.h"
#include "TrackControlListener.h"
#include "model/Track.h"
#include "ui/common/DecibelSlider.h"
#include "ui/common/FileChooserControl.h"

using namespace std;
using namespace juce;

namespace trackman {

class TrackControl : public Component, public SliderListener {
  public:
    TrackControl(Track &track);
    ~TrackControl();

    static const int getPreferredWidth() { return 105; }

    void update();

    void addListener(TrackControlListener *listener);
    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;
    void mouseUp(const MouseEvent &event) override;

    //==============================================================================
    // SliderListener
    void sliderClicked() override;

  private:
    Track &track;

    DecibelSlider decibelSlider;
    TextButton muteButton;
    TextButton soloButton;
    Label trackNameLabel;
    Label trackNumberLabel;
    LevelMeterLookAndFeel levelMeterLookAndFeel;
    foleys::LevelMeter levelMeter;

    bool draggingSlider = false;
    float previousLevel = Decibels::decibelsToGain<float>(0.0);
    list<TrackControlListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void soloButtonClicked();

    void notifyNameChanged();
    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();
    void notifySoloToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackControl)
};

} // namespace trackman
