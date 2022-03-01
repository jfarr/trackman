#pragma once

#include "JuceHeader.h"

#include "controls/common/DecibelSlider.h"

class MasterTrackListener {
  public:
    virtual void levelChanged(float level) = 0;
    virtual void muteToggled() = 0;
};

class MasterTrackControl : public juce::Component {
  public:
    MasterTrackControl();
    ~MasterTrackControl();

    void addListener(MasterTrackListener *listener);
    void removeListener(MasterTrackListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;
    bool muted = false;
    std::list<MasterTrackListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void notifyLevelChanged(float level);
    void notifyMuteToggled();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterTrackControl)
};
