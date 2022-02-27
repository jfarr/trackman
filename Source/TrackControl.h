#pragma once

#include <JuceHeader.h>
#include "DecibelSlider.h"

class TrackControl : public juce::Component
{
public:
    TrackControl(juce::String trackName);
    ~TrackControl();

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::String trackName;
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;
    float level;
    bool muted;

    void createControls();
    void muteButtonClicked();
};
