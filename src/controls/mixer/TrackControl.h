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
    // SliderListener
    void onSliderClick() override;

    //==============================================================================
    // MouseListener
    void mouseUp(const juce::MouseEvent &event) override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    Track &track;

    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
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
    void notifyNameChanged();
    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackControl)
};
