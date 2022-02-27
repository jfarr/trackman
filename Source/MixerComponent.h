#pragma once

#include <JuceHeader.h>
#include "MasterTrackControl.h"
#include "Track.h"
#include "TrackControl.h"
#include "TransportControl.h"


class MixerComponent : public juce::Component
{
public:
    MixerComponent();
    ~MixerComponent();

    void addTrack(Track& track);

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::AudioTransportSource transportSource;
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<Track*> tracks;

    void createControls();
};
