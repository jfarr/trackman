#include "MasterTrackControl.h"

#include "common/listutil.h"

MasterTrackControl::MasterTrackControl(Mixer &mixer) : mixer(mixer) {
    createControls();
    setSize(100, 100);
}

MasterTrackControl::~MasterTrackControl() {}

void MasterTrackControl::createControls() {
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.setValue(0.0);
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText("MASTER", juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(
        juce::Label::backgroundColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
}

void MasterTrackControl::update() {
    muteButton.setColour(juce::TextButton::buttonColourId,
        mixer.getMasterMute() ? juce::Colours::red
                              : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MasterTrackControl::setLevel(float level) {
    previousLevel = level;
    decibelSlider.setValue(juce::Decibels::gainToDecibels(level));
}

void MasterTrackControl::onSliderClick() { draggingSlider = true; }

void MasterTrackControl::mouseUp(const juce::MouseEvent &event) {
    if (event.eventComponent == &decibelSlider) {
        draggingSlider = false;
        decibelSliderChanged();
    }
}

void MasterTrackControl::paint(juce::Graphics &g) {
    g.fillAll(juce::Colour{0xff282828});
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(juce::Colour{0xff282828});
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
}

void MasterTrackControl::resized() {
    auto area = getLocalBounds();
    auto sliderWidth = 75;
    auto buttonSize = 25;
    auto labelHeight = 20;
    auto margin = 3;
    channelLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(margin));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

void MasterTrackControl::decibelSliderChanged() {
    auto level = juce::Decibels::decibelsToGain((float)decibelSlider.getValue());
    notifyLevelChanged(level);
    if (!draggingSlider && level != previousLevel) {
        notifyLevelChangeFinalized(previousLevel);
        previousLevel = level;
    }
}

void MasterTrackControl::muteButtonClicked() {
    notifyMuteToggled();
    muteButton.setColour(juce::TextButton::buttonColourId,
        mixer.getMasterMute() ? juce::Colours::red
                                   : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MasterTrackControl::addListener(MasterTrackListener *listener) {
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void MasterTrackControl::removeListener(MasterTrackListener *listener) { listeners.remove(listener); }

void MasterTrackControl::notifyLevelChanged(float level) {
    for (MasterTrackListener *listener : listeners) {
        listener->masterLevelChanged(level);
    }
}

void MasterTrackControl::notifyLevelChangeFinalized(float previousLevel) {
    for (MasterTrackListener *listener : listeners) {
        listener->masterLevelChangeFinalized(previousLevel);
    }
}

void MasterTrackControl::notifyMuteToggled() {
    for (MasterTrackListener *listener : listeners) {
        listener->masterMuteToggled();
    }
}
