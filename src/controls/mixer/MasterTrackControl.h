#pragma once

#include "JuceHeader.h"

#include "MasterTrackListener.h"
#include "controls/common/DecibelSlider.h"
#include "model/Mixer.h"

class MasterTrackControl : public juce::Component, public SliderListener {
  public:
    MasterTrackControl(Mixer &mixer);
    ~MasterTrackControl();

    void update();

    void setLevel(float level);

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // SliderListener
    void onSliderClick() override;

    //==============================================================================
    // MouseListener
    void mouseUp(const juce::MouseEvent &event) override;

  private:
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;

    Mixer &mixer;
    bool draggingSlider = false;
    float previousLevel = juce::Decibels::decibelsToGain<float>(0.0);
    std::list<MasterTrackListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterTrackControl)
};
