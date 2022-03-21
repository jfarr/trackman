#include "MasterTrackControl.h"
#include "common/listutil.h"

namespace trackman {

MasterTrackControl::MasterTrackControl(Mixer &mixer, LevelMeterSource &meterSource)
    : mixer(mixer), previousLevel(mixer.getMasterLevelGain()), levelMeter(LevelMeter::MeterFlags::Minimal) {
    levelMeter.setMeterSource(&meterSource);
    createControls();
    update();
    setSize(getPreferredWidth(), 100);
}

MasterTrackControl::~MasterTrackControl() { levelMeter.setLookAndFeel(nullptr); }

void MasterTrackControl::createControls() {
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText("MASTER", dontSendNotification);
    channelLabel.setJustificationType(Justification(Justification::horizontallyCentred));
    channelLabel.setColour(Label::backgroundColourId, getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    levelMeter.setLookAndFeel(&levelMeterLookAndFeel);

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
    addAndMakeVisible(levelMeter);
}

void MasterTrackControl::update() {
    auto level = mixer.getMasterLevelGain();
    if (level != previousLevel) {
        previousLevel = level;
        decibelSlider.setValue(Decibels::gainToDecibels(level));
    }
    muteButton.setColour(TextButton::buttonColourId,
        mixer.isMasterMuted() ? Colours::red : getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MasterTrackControl::setLevel(float level) {
    previousLevel = level;
    decibelSlider.setValue(Decibels::gainToDecibels(level));
}

void MasterTrackControl::paint(Graphics &g) {
    g.fillAll(Colour{0xff282828});
    g.setColour(Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(Colour{0xff282828});
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
    auto buttonsHeight = 30;
    g.setColour(Colours::lightgrey);
    g.fillRect(0, 0, getWidth(), buttonsHeight);
    g.setColour(Colours::dimgrey);
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

void MasterTrackControl::mouseUp(const MouseEvent &event) {
    if (event.eventComponent == &decibelSlider) {
        draggingSlider = false;
        decibelSliderChanged();
    }
}

void MasterTrackControl::sliderClicked() { draggingSlider = true; }

void MasterTrackControl::decibelSliderChanged() {
    auto level = Decibels::decibelsToGain((float)decibelSlider.getValue());
    notifyLevelChanged(level);
    if (!draggingSlider && level != previousLevel) {
        notifyLevelChangeFinalized(previousLevel);
        previousLevel = level;
    }
}

void MasterTrackControl::muteButtonClicked() {
    update();
    notifyMuteToggled();
}

void MasterTrackControl::addListener(MasterTrackListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
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

} // namespace trackman
