#include "MixerComponent.h"

MixerComponent::MixerComponent() : transportControl(transportSource)
{
    createControls();
    setSize(800, 250);
}

MixerComponent::~MixerComponent()
{
}

void MixerComponent::createControls()
{
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterChannelControl);
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
    masterChannelControl.setBounds(area.removeFromLeft(masterChannelControl.getWidth()));
}