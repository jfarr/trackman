#pragma once

#include "JuceHeader.h"

#include "controls/common/DecibelSlider.h"
#include "controls/common/FileChooserControl.h"
#include "controls/mixer/TrackControlListener.h"
#include "model/Track.h"

class TrackControl : public juce::Component, public SliderListener {
  public:
    TrackControl(Track &track);
    ~TrackControl();

    juce::String getTrackName() const { return track.getName(); }

    void setLevel(float level);
    void setSelected(bool newSelected);
    void setListener(class FileListener *newListener) { listener = newListener; }

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
    juce::Label trackLabel;
    juce::Label channelLabel;
    bool muted = false;
    bool draggingSlider = false;
    float previousLevel = juce::Decibels::decibelsToGain<float>(0.0);
    bool selected = false;
    std::list<TrackControlListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void notifyLevelChanged(float level);
    void notifyLevelChangeFinalized(float previousLevel);
    void notifyMuteToggled();

    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton openButton;
    void openButtonClicked();
    FileListener *listener = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackControl)
};
