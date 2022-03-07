#include "TrackControl.h"

#include "common/listutil.h"

TrackControl::TrackControl(Track &track) : track(track) {
    createControls();
    setSize(getPreferredWidth(), 100);
}

TrackControl::~TrackControl() {}

void TrackControl::createControls() {
    previousLevel = track.getLevelGain();
    decibelSlider.setValue(juce::Decibels::gainToDecibels(track.getLevelGain()));
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.setColour(juce::TextButton::buttonColourId,
        track.isMuted() ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText(track.getName(), juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    channelLabel.setColour(juce::Label::textColourId, juce::Colour{0xff282828});

    trackLabel.setText("" /*track.getName()*/, juce::dontSendNotification);
    trackLabel.setColour(juce::Label::textColourId, juce::Colours::grey);

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(trackLabel);
    addAndMakeVisible(channelLabel);
}

void TrackControl::update() {
    previousLevel = track.getLevelGain();
    decibelSlider.setValue(juce::Decibels::gainToDecibels(track.getLevelGain()));
    muteButton.setColour(juce::TextButton::buttonColourId,
        track.isMuted() ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackControl::onSliderClick() { draggingSlider = true; }

void TrackControl::mouseUp(const juce::MouseEvent &event) {
    if (event.eventComponent == &decibelSlider) {
        draggingSlider = false;
        decibelSliderChanged();
    }
}

void TrackControl::paint(juce::Graphics &g) {
    auto bgColor = juce::Colour{0xff282828};
    auto buttonsHeight = 30;
    auto labelHeight = 25;
    g.fillAll(bgColor);
    g.setColour(track.isSelected() ? juce::Colours::lightgrey : juce::Colours::grey);
    g.fillRect(0, 0, getWidth(), buttonsHeight + labelHeight);
    g.fillRect(0, getHeight() - labelHeight, getWidth(), labelHeight);
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(bgColor);
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(0, buttonsHeight - 1, getWidth() - 1, 1);
    g.setColour(bgColor);
    g.fillRect(0, buttonsHeight, getWidth() - 1, 1);
}

void TrackControl::resized() {
    auto area = getLocalBounds();
    auto sliderWidth = 45;
    auto buttonSize = 25;
    auto buttonsHeight = 30;
    auto labelHeight = 25;
    auto margin = 3;
    area.removeFromTop(buttonsHeight + labelHeight + margin);
    channelLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    trackLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

void TrackControl::decibelSliderChanged() {
    auto level = juce::Decibels::decibelsToGain((float)decibelSlider.getValue());
    notifyLevelChanged(level);
    if (!draggingSlider && level != previousLevel) {
        notifyLevelChangeFinalized(previousLevel);
        previousLevel = level;
    }
}

void TrackControl::muteButtonClicked() {
    notifyMuteToggled();
    muteButton.setColour(juce::TextButton::buttonColourId,
        track.isMuted() ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackControl::addListener(TrackControlListener *listener) {
    if (!listContains(listeners, listener))
        listeners.push_front(listener);
}

void TrackControl::removeListener(TrackControlListener *listener) { listeners.remove(listener); }

void TrackControl::notifyLevelChanged(float level) {
    for (TrackControlListener *listener : listeners) {
        listener->levelChanged(level);
    }
}

void TrackControl::notifyLevelChangeFinalized(float previousLevel) {
    for (TrackControlListener *listener : listeners) {
        listener->levelChangeFinalized(track, previousLevel);
    }
}

void TrackControl::notifyMuteToggled() {
    for (TrackControlListener *listener : listeners) {
        listener->muteToggled(track);
    }
}
