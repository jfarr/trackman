#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

#include "MasterLevelMeterLookAndFeel.h"
#include "MasterTrackListener.h"
#include "model/Mixer.h"
#include "ui/common/DecibelSlider.h"

class MasterTrackControl : public juce::Component, public SliderListener {
  public:
    MasterTrackControl(Mixer &mixer, foleys::LevelMeterSource &meterSource);
    ~MasterTrackControl();

    static int getPreferredWidth() { return 150; }

    void update();

    void setLevel(float level);

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent &event) override;

    //==============================================================================
    // SliderListener
    void sliderClicked() override;

  private:
    Mixer &mixer;

    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;
    MasterLevelMeterLookAndFeel levelMeterLookAndFeel;
    foleys::LevelMeter levelMeter;

    bool draggingSlider = false;
    float previousLevel;
    std::list<MasterTrackListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();

    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterTrackControl)
};