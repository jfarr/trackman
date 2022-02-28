#include "TrackControl.h"

#include "listutil.h"

TrackControl::TrackControl(juce::String trackName)
    : trackName(trackName),
      level(juce::Decibels::decibelsToGain<float>(0.0)),
      muted(false) {
    createControls();
    setSize(100, 100);
}

TrackControl::~TrackControl() {}

void TrackControl::createControls() {
    decibelSlider.onValueChange = [this] {
        level = juce::Decibels::decibelsToGain((float)decibelSlider.getValue());
    };
    decibelSlider.setValue(juce::Decibels::gainToDecibels(level));

    muteButton.setButtonText("M");
    muteButton.setTooltip("mute");
    muteButton.onClick = [this] { muteButtonClicked(); };

    channelLabel.setText(trackName, juce::dontSendNotification);
    channelLabel.setJustificationType(
        juce::Justification(juce::Justification::horizontallyCentred));
    channelLabel.setColour(juce::Label::backgroundColourId,
                           juce::Colours::grey);

    openButton.setButtonText("...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(decibelSlider);
    addAndMakeVisible(muteButton);
    addAndMakeVisible(channelLabel);
    addAndMakeVisible(openButton);
}

void TrackControl::paint(juce::Graphics &g) {
    auto bgColor = juce::Colour{0xff282828};
    g.fillAll(bgColor);
    g.setColour(juce::Colours::black);
    g.fillRect(getWidth() - 1, 0, 1, getHeight());
    g.setColour(bgColor);
    g.fillRect(getWidth() - 1, 0, 0, getHeight());
}

void TrackControl::resized() {
    auto area = getLocalBounds();
    auto sliderWidth = 75;
    auto buttonSize = 25;
    auto labelHeight = 15;
    auto margin = 3;
    channelLabel.setBounds(
        area.removeFromBottom(labelHeight).withTrimmedRight(1));
    decibelSlider.setBounds(area.removeFromLeft(sliderWidth).reduced(margin));
    auto buttonArea = area.removeFromLeft(buttonSize);
    muteButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
    openButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

void TrackControl::muteButtonClicked() {
    muted = !muted;
    muteButton.setColour(juce::TextButton::buttonColourId,
                         muted
                             ? juce::Colours::red
                             : getLookAndFeel().findColour(
                                   juce::ResizableWindow::backgroundColourId));
}

void TrackControl::openButtonClicked() {
    chooser = std::make_unique<juce::FileChooser>("Select a file to play...",
                                                  juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode |
                        juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();

        if (file != juce::File{}) {
            listener->fileChosen(file);
        }
    });
}
