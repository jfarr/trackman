#include "MasterTrackControl.h"

#include "common/listutil.h"

MasterTrackControl::MasterTrackControl(Mixer &mixer, foleys::LevelMeterSource &meterSource)
    : mixer(mixer), levelMeterLookAndFeel(mixer), levelMeter(foleys::LevelMeter::MeterFlags::Minimal) {
    levelMeter.setMeterSource(&meterSource);
    createControls();
    setSize(getPreferredWidth(), 100);
}

MasterTrackControl::~MasterTrackControl() {
    levelMeter.setLookAndFeel(nullptr);
}

void MasterTrackControl::createControls() {
    previousLevel = mixer.getMasterLevelGain();
    decibelSlider.setValue(juce::Decibels::gainToDecibels(mixer.getMasterLevelGain()));
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText("MASTER", juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(
        juce::Label::backgroundColourId, getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    levelMeter.setLookAndFeel(&levelMeterLookAndFeel);

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
    addAndMakeVisible(levelMeter);
}

void MasterTrackControl::update() {
    previousLevel = mixer.getMasterLevelGain();
    decibelSlider.setValue(juce::Decibels::gainToDecibels(mixer.getMasterLevelGain()));
    muteButton.setColour(juce::TextButton::buttonColourId,
        mixer.isMasterMuted() ? juce::Colours::red
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
    auto buttonsHeight = 30;
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(0, 0, getWidth(), buttonsHeight);
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(0, buttonsHeight - 1, getWidth(), 1);
}

void MasterTrackControl::resized() {
    auto area = getLocalBounds();
    auto buttonsHeight = 30;
    auto labelHeight = 25;
    auto sliderWidth = 45;
    auto meterWidth = 75;
    auto buttonSize = 25;
    auto margin = 3;
    area.removeFromTop(buttonsHeight + margin);
    area.removeFromLeft(5);
    channelLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    levelMeter.setBounds(area.removeFromLeft(meterWidth).reduced(5));
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
        mixer.isMasterMuted() ? juce::Colours::red
                              : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MasterTrackControl::addListener(MasterTrackListener *listener) {
    if (!listContains(listeners, listener))
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
