#include "MasterTrackControl.h"
#include "listutil.h"


MasterTrackControl::MasterTrackControl()
    : level(juce::Decibels::decibelsToGain<float>(0.0)), muted(false)
{
    createControls();
    setSize(100, 100);
}

MasterTrackControl::~MasterTrackControl()
{
}

void MasterTrackControl::createControls()
{
    decibelSlider.setSliderStyle(juce::Slider::LinearVertical);
    decibelSlider.setRange(-100, 12);
    decibelSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 65, 14);
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.setValue(juce::Decibels::gainToDecibels(level));

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText("MASTER", juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(juce::Label::backgroundColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
}

void MasterTrackControl::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour{ 0xff282828 });
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(juce::Colour{ 0xff282828 });
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
}

void MasterTrackControl::resized()
{
    auto area = getLocalBounds();
    auto sliderWidth = 75;
    auto buttonSize = 25;
    auto labelHeight = 15;
    auto margin = 3;
    channelLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(margin));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

void MasterTrackControl::decibelSliderChanged()
{
    level = juce::Decibels::decibelsToGain((float)decibelSlider.getValue());
    notifyLevelChanged();
}

void MasterTrackControl::muteButtonClicked()
{
    muted = !muted;
    muteButton.setColour(juce::TextButton::buttonColourId, muted ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    notifyMuteChanged();
}

void MasterTrackControl::addListener(MasterTrackListener* listener)
{
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void MasterTrackControl::removeListener(MasterTrackListener* listener)
{
    listeners.remove(listener);
}

void MasterTrackControl::notifyLevelChanged()
{
    for (std::list<MasterTrackListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
    {
        MasterTrackListener& listener = **i;
        listener.levelChanged(level);
    }
}

void MasterTrackControl::notifyMuteChanged()
{
    for (std::list<MasterTrackListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
    {
        MasterTrackListener& listener = **i;
        listener.muteChanged(muted);
    }
}
