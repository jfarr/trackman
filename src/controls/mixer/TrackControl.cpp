#include "TrackControl.h"

#include "common/listutil.h"

TrackControl::TrackControl(Track &track)
    : track(track), levelMeter(foleys::LevelMeter::MeterFlags::Minimal) {
    createControls();
    update();
    setSize(getPreferredWidth(), 100);
}

TrackControl::~TrackControl() { levelMeter.setLookAndFeel(nullptr); }

void TrackControl::createControls() {
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    soloButton.setButtonText("S");
    soloButton.setTooltip("solo");
    soloButton.onClick = [this] { soloButtonClicked(); };

    trackNameLabel.setJustificationType(juce::Justification::horizontallyCentred);
    trackNameLabel.setColour(juce::Label::textColourId, juce::Colours::grey);
    trackNameLabel.setEditable(true);
    trackNameLabel.onTextChange = [this] { notifyNameChanged(); };

    trackNumberLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    trackNumberLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
    trackNumberLabel.setColour(juce::Label::textColourId, juce::Colour{0xff282828});

    levelMeter.setLookAndFeel(&levelMeterLookAndFeel);
    levelMeter.setFixedNumChannels(2);
    if (track.getMeterSource() != nullptr) {
        levelMeter.setMeterSource(track.getMeterSource());
    }

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(soloButton);
    addAndMakeVisible(trackNameLabel);
    addAndMakeVisible(trackNumberLabel);
    addAndMakeVisible(levelMeter);
}

void TrackControl::update() {
    previousLevel = track.getLevelGain();
    decibelSlider.setValue(juce::Decibels::gainToDecibels(track.getLevelGain()));
    muteButton.setColour(juce::TextButton::buttonColourId,
        track.isMuted() ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    soloButton.setColour(juce::TextButton::buttonColourId,
        track.isSoloed() ? juce::Colours::orange
                         : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    trackNameLabel.setText(track.getName(), juce::dontSendNotification);
    trackNumberLabel.setText("Track " + juce::String(track.getNumber()), juce::dontSendNotification);
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
    auto meterWidth = 35;
    auto buttonSize = 25;
    auto buttonsHeight = 30;
    auto labelHeight = 25;
    auto margin = 3;
    area.removeFromTop(buttonsHeight + labelHeight + margin);
    trackNumberLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    trackNameLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    levelMeter.setBounds(area.removeFromLeft(meterWidth).reduced(5));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
    soloButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
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

void TrackControl::soloButtonClicked() {
    notifySoloToggled();
    soloButton.setColour(juce::TextButton::buttonColourId,
        track.isSoloed() ? juce::Colours::orange
                         : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackControl::addListener(TrackControlListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void TrackControl::removeListener(TrackControlListener *listener) { listeners.remove(listener); }

void TrackControl::notifyNameChanged() {
    for (TrackControlListener *listener : listeners) {
        listener->trackNameChanged(track, trackNameLabel.getText());
    }
}

void TrackControl::notifyLevelChanged(float level) {
    for (TrackControlListener *listener : listeners) {
        listener->trackLevelChanged(track, level);
    }
}

void TrackControl::notifyLevelChangeFinalized(float previousLevel) {
    for (TrackControlListener *listener : listeners) {
        listener->trackLevelChangeFinalized(track, previousLevel);
    }
}

void TrackControl::notifyMuteToggled() {
    for (TrackControlListener *listener : listeners) {
        listener->trackMuteToggled(track);
    }
}

void TrackControl::notifySoloToggled() {
    for (TrackControlListener *listener : listeners) {
        listener->trackSoloToggled(track);
    }
}
