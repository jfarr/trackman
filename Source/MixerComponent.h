#pragma once

#include <JuceHeader.h>
#include "TransportControl.h"


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
    //TransportControl transportControl;
};
