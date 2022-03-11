#pragma once

#include "JuceHeader.h"

#include "LevelMeterLookAndFeel.h"
#include "controls/common/DecibelSlider.h"
#include "controls/common/FileChooserControl.h"
#include "controls/mixer/TrackControlListener.h"
#include "model/Track.h"

class TrackControl : public juce::Component, public SliderListener {
  public:
    TrackControl(Track &track);
    ~TrackControl();

    int getPreferredWidth() const { return std::max(getWidth(), 105); }

    void update();

    void addListener(TrackControlListener *listener);
    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent &event) override;

    //==============================================================================
    // SliderListener
    void sliderClicked() override;

  private:
    Track &track;

    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::TextButton soloButton;
    juce::Label trackNameLabel;
    juce::Label trackNumberLabel;
    LevelMeterLookAndFeel levelMeterLookAndFeel;
    foleys::LevelMeter levelMeter;

    bool draggingSlider = false;
    float previousLevel = juce::Decibels::decibelsToGain<float>(0.0);
    std::list<TrackControlListener *> listeners;

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
