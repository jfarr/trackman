#pragma once

#include <JuceHeader.h>
#include "TransportControl.h"
#include "MasterChannelControl.h"


class MixerComponent : public juce::Component
{
public:
    MixerComponent();
    ~MixerComponent();

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterChannelControl masterChannelControl;

    void createControls();
};
