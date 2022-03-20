#include "TransportPanel.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TransportPanel::TransportPanel(DesktopController &desktopController)
    : desktopController(desktopController),
      transportControl(desktopController.getProject().getTransport().getTransportSource(), true,
          &desktopController.getMidiRecorder(), &desktopController.getProject().getTrackList()) {

    createControls();
    update();
}

TransportPanel::~TransportPanel() noexcept {
    denominatorSelect.setLookAndFeel(nullptr);
}

void TransportPanel::createControls() {
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
}

void TransportPanel::update() {
    tempoText.setText(juce::String(desktopController.getProject().getTempo()), juce::dontSendNotification);
    numeratorText.setText(
        juce::String(desktopController.getProject().getTimeSignature().getNumerator()), juce::dontSendNotification);
    denominatorSelect.setSelectedId(
        desktopController.getProject().getTimeSignature().getDenominator(), juce::dontSendNotification);
}

void TransportPanel::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::lightgrey);
    auto area = getLocalBounds();
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(area.removeFromBottom(1));
}

void TransportPanel::resized() {
    auto area = getLocalBounds();
    auto tempoArea = area.withTrimmedLeft(transportWidth);
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
    transportControl.setBounds(area.reduced(transportMargin));
}

} // namespace trackman
