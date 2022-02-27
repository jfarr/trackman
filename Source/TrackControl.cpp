#include "TrackControl.h"


TrackControl::TrackControl(juce::String trackName) : trackName(trackName), level(juce::Decibels::decibelsToGain<float>(0.0)), muted(false)
{
    createControls();
    setSize(125, 100);
}

TrackControl::~TrackControl()
{
}

void TrackControl::createControls()
{
    decibelSlider.setSliderStyle(juce::Slider::LinearVertical);
    decibelSlider.setRange(-100, 12);
    decibelSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 80, 20);
    decibelSlider.onValueChange = [this] { level = juce::Decibels::decibelsToGain((float)decibelSlider.getValue()); };
    decibelSlider.setValue(juce::Decibels::gainToDecibels(level));

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText(trackName, juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
}

void TrackControl::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour{ 0xff282828 });
}

void TrackControl::resized()
{
    auto area = getLocalBounds();
    auto sliderWidth = 100;
    auto buttonSize = 25;
    auto labelHeight = 15;
    auto margin = 2;
    channelLabel.setBounds(area.removeFromBottom(labelHeight).reduced(margin));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(margin));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

void TrackControl::muteButtonClicked()
{
    muted = !muted;
    muteButton.setColour(juce::TextButton::buttonColourId, muted ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}
