#include "TrackControl.h"

#include "common/listutil.h"

TrackControl::TrackControl(Track &track) : track(track) {
    createControls();
    setSize(100, 100);
}

TrackControl::~TrackControl() {}

void TrackControl::createControls() {
    decibelSlider.onValueChange = [this] { decibelSliderChanged(); };
    decibelSlider.setValue(0.0);
    decibelSlider.addMouseListener(this, false);
    decibelSlider.setListener(this);

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText(track.getName(), juce::dontSendNotification);
    channelLabel.setJustificationType(juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(juce::Label::backgroundColourId, selected ? juce::Colours::lightgrey : juce::Colours::grey);
    channelLabel.setColour(juce::Label::textColourId, juce::Colour{0xff282828});

    trackLabel.setColour(juce::Label::textColourId, juce::Colours::grey);

    openButton.setButtonText("...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(trackLabel);
    addAndMakeVisible(channelLabel);
    addAndMakeVisible(openButton);
}

void TrackControl::setLevel(float level) {
    previousLevel = level;
    decibelSlider.setValue(juce::Decibels::gainToDecibels(level));
}

void TrackControl::onSliderClick() { draggingSlider = true; }

void TrackControl::mouseUp(const juce::MouseEvent &event) {
    if (event.eventComponent == &decibelSlider) {
        draggingSlider = false;
        decibelSliderChanged();
    }
}

void TrackControl::setSelected(bool newSelected) {
    selected = newSelected;
    channelLabel.setColour(juce::Label::backgroundColourId, selected ? juce::Colours::lightgrey : juce::Colours::grey);
    repaint();
}

void TrackControl::paint(juce::Graphics &g) {
    auto bgColor = juce::Colour{0xff282828};
    auto labelHeight = 20;
    g.fillAll(bgColor);
    g.setColour(selected ? juce::Colours::lightgrey : juce::Colours::grey);
    g.fillRect(0, 0, getWidth(), labelHeight);
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(bgColor);
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
}

void TrackControl::resized() {
    auto area = getLocalBounds();
    auto sliderWidth = 75;
    auto buttonSize = 25;
    auto labelHeight = 20;
    auto margin = 3;
    area.removeFromTop(labelHeight);
    channelLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    trackLabel.setBounds(area.removeFromBottom(labelHeight).withTrimmedRight(1));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
    openButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
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
    muted = !muted;
    muteButton.setColour(juce::TextButton::buttonColourId,
        muted ? juce::Colours::red : getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    notifyMuteToggled();
}

void TrackControl::openButtonClicked() {
    chooser = std::make_unique<juce::FileChooser>("Select a file to play...", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();

        if (listener != nullptr && file != juce::File{}) {
            trackLabel.setText(file.getFileName(), juce::dontSendNotification);
            listener->fileChosen(file);
        }
    });
}

void TrackControl::addListener(TrackControlListener *listener) {
    if (!listContains(listener, listeners))
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
        listener->levelChangeFinalized(*this, previousLevel);
    }
}

void TrackControl::notifyMuteToggled() {
    for (TrackControlListener *listener : listeners) {
        listener->muteToggled();
    }
}
