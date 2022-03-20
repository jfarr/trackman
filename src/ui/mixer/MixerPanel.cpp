#include "MixerPanel.h"
#include "ui/desktop/DesktopController.h"

MixerPanel::MixerPanel(DesktopController &desktopController, foleys::LevelMeterSource &meterSource)
    : desktopController(desktopController), masterTrackControl(desktopController.getMixer(), meterSource),
      mixerViewport(desktopController.getMixerController().getViewport()),
      transportControl(desktopController.getMixer().getTransportSource(), true, &desktopController.getMidiRecorder(),
          &desktopController.getProject().getTrackList()) {
    createControls();
    update();
    masterTrackControl.addListener(&desktopController);
    setSize(preferredWidth, preferredHeight);
}

MixerPanel::~MixerPanel() {
    masterTrackControl.removeListener(&desktopController);
    denominatorSelect.setLookAndFeel(nullptr);
}

void MixerPanel::createControls() {
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setJustificationType(juce::Justification::centredRight);
    tempoText.setJustificationType(juce::Justification::centredLeft);
    tempoText.onTextChange = [this] { desktopController.tempoChanged(tempoText.getText().getFloatValue()); };

    numeratorText.setJustificationType(juce::Justification::centredRight);
    numeratorText.onTextChange = [this] { desktopController.numeratorChanged(numeratorText.getText().getIntValue()); };

    timeSignatureDivider.setText("/", juce::dontSendNotification);
    timeSignatureDivider.setInterceptsMouseClicks(false, false);
    timeSignatureDivider.setJustificationType(juce::Justification::centred);

    denominatorSelect.setLookAndFeel(&denominatorLF);
    denominatorSelect.addItem("4", 4);
    denominatorSelect.addItem("8", 8);
    denominatorSelect.addItem("16", 16);
    denominatorSelect.onChange = [this] { desktopController.denominatorChanged(denominatorSelect.getSelectedId()); };

    mixerViewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);

    addAndMakeVisible(transportControl);
    addAndMakeVisible(tempoLabel);
    addAndMakeVisible(tempoText);
    addAndMakeVisible(numeratorText);
    addAndMakeVisible(timeSignatureDivider);
    addAndMakeVisible(denominatorSelect);
    addAndMakeVisible(masterTrackControl);
    addAndMakeVisible(mixerViewport);
}

void MixerPanel::update() {
    tempoText.setText(juce::String(desktopController.getProject().getTempo()), juce::dontSendNotification);
    numeratorText.setText(
        juce::String(desktopController.getProject().getTimeSignature().getNumerator()), juce::dontSendNotification);
    denominatorSelect.setSelectedId(
        desktopController.getProject().getTimeSignature().getDenominator(), juce::dontSendNotification);
}

//==============================================================================
void MixerPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    auto area = getLocalBounds();
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(area.removeFromTop(transportHeight));
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(area.removeFromTop(1));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
    auto tempoArea = area.withTrimmedLeft(transportWidth).withHeight(transportHeight);
    tempoLabel.setBounds(
        tempoArea.removeFromLeft(50).withTrimmedTop(transportMargin - 1).withTrimmedBottom(transportMargin + 1));
    tempoText.setBounds(
        tempoArea.removeFromLeft(45).withTrimmedTop(transportMargin).withTrimmedBottom(transportMargin));
    numeratorText.setBounds(
        tempoArea.removeFromLeft(25).withTrimmedTop(transportMargin + 2).withTrimmedBottom(transportMargin + 2));
    timeSignatureDivider.setBounds(tempoArea.removeFromLeft(15)
                                       .withTrimmedLeft(-5)
                                       .withTrimmedTop(transportMargin - 1)
                                       .withTrimmedBottom(transportMargin + 1));
    denominatorSelect.setBounds(tempoArea.removeFromLeft(47)
                                    .withTrimmedLeft(-5)
                                    .withTrimmedTop(transportMargin + 2)
                                    .withTrimmedBottom(transportMargin + 2));
    transportControl.setBounds(area.removeFromTop(transportHeight).reduced(transportMargin));
    area.removeFromTop(1);
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getPreferredWidth()));
    mixerViewport.setBounds(area);
}
