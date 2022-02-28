#pragma once

#include <JuceHeader.h>

#include "DecibelSlider.h"

class MasterTrackListener {
   public:
    virtual void levelChanged(float level) = 0;
    virtual void muteChanged(bool muted) = 0;
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
    float level;
    bool muted;
    std::list<MasterTrackListener *> listeners;

    void createControls();
    void decibelSliderChanged();
    void muteButtonClicked();
    void notifyLevelChanged();
    void notifyMuteChanged();
};
