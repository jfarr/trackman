#include "MixerComponent.h"

MixerComponent::MixerComponent()
    : transportControl(transportSource)
{
    createControls();
    setSize(800, 250);
}

MixerComponent::~MixerComponent()
{
}

void MixerComponent::addTrack(Track& track)
{
    tracks.push_back(&track);
    removeAllChildren();
    createControls();
    for (std::list<Track*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
    {
        Track& track = **i;
        addAndMakeVisible(track.getTrackControl());
    }
    resized();
}

void MixerComponent::createControls()
{
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
}

void MixerComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MixerComponent::resized()
{
    auto area = getLocalBounds();
    auto buttonHeight = 35;
    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(buttonHeight).reduced(transportMargin));
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getWidth()));
    for (std::list<Track*>::iterator i = tracks.begin(); i != tracks.end(); ++i)
    {
        Track& track = **i;
        track.getTrackControl().setBounds(area.removeFromLeft(track.getTrackControl().getWidth()));
    }
}
