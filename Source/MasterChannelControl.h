#pragma once

#include <JuceHeader.h>
#include "DecibelSlider.h"

class MasterChannelControl : public juce::Component
{
public:
    MasterChannelControl();
    ~MasterChannelControl();

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    DecibelSlider decibelSlider;
    juce::TextButton muteButton;
    juce::Label channelLabel;
    float level;
    bool muted;

    void createControls();
    void muteButtonClicked();
};
