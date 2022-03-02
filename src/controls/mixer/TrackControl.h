#pragma once

#include "JuceHeader.h"

#include "controls/common/DecibelSlider.h"
#include "controls/common/FileChooserControl.h"

class TrackControlListener {
  public:
    virtual void levelChanged(float level) = 0;
    virtual void muteToggled() = 0;
};

class TrackControl : public juce::Component {
  public:
    TrackControl(juce::String trackName);
    ~TrackControl();

    void setSelected(bool newSelected);
    void setListener(class FileListener *newListener) { listener = newListener; }

    void addListener(TrackControlListener *listener);
    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    juce::String trackName;
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label trackLabel;
    juce::Label channelLabel;
    bool muted = false;
    bool selected = false;
    std::list<TrackControlListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void notifyLevelChanged(float level);
    void notifyMuteToggled();

    std::unique_ptr<juce::FileChooser> chooser;
    juce::TextButton openButton;
    void openButtonClicked();
    FileListener *listener = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackControl)
};
