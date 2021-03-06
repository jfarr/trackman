#include "TransportPanel.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TransportPanel::TransportPanel(DesktopController &desktopController)
    : desktopController(desktopController),
      transportControl(
          desktopController.getProject().getTransport().getTransportSource(), true,
          [&desktopController]() { return desktopController.canRecord(); },
          [&desktopController]() { return desktopController.getProject().getCurrentPosition(); }) {

    createControls();
    update();
}

TransportPanel::~TransportPanel() noexcept { denominatorSelect.setLookAndFeel(nullptr); }

void TransportPanel::createControls() {

    transportControl.onPlaybackStarted = [this] { desktopController.playbackStarted(); };
    transportControl.onPlaybackStopped = [this] { desktopController.playbackStopped(); };
    transportControl.onLoopingChanged = [this](bool shouldLoop) { desktopController.loopingChanged(shouldLoop); };
    transportControl.onRecordingStarted = [this] { desktopController.recordingStarted(); };
    transportControl.onRecordingStopped = [this] { desktopController.recordingStopped(); };
    transportControl.onRecordingPaused = [this] { desktopController.recordingPaused(); };

    tempoLabel.setText("Tempo", dontSendNotification);
    tempoLabel.setJustificationType(Justification::centredRight);
    tempoText.setJustificationType(Justification::centredLeft);
    tempoText.onTextChange = [this] { desktopController.tempoChanged(tempoText.getText().getFloatValue()); };

    numeratorText.setJustificationType(Justification::centredRight);
    numeratorText.onTextChange = [this] { desktopController.numeratorChanged(numeratorText.getText().getIntValue()); };

    timeSignatureDivider.setText("/", dontSendNotification);
    timeSignatureDivider.setInterceptsMouseClicks(false, false);
    timeSignatureDivider.setJustificationType(Justification::centred);

    denominatorSelect.setLookAndFeel(&denominatorLF);
    denominatorSelect.addItem("4", 4);
    denominatorSelect.addItem("8", 8);
    denominatorSelect.addItem("16", 16);
    denominatorSelect.onChange = [this] { desktopController.denominatorChanged(denominatorSelect.getSelectedId()); };

    addAndMakeVisible(transportControl);
    addAndMakeVisible(tempoLabel);
    addAndMakeVisible(tempoText);
    addAndMakeVisible(numeratorText);
    addAndMakeVisible(timeSignatureDivider);
    addAndMakeVisible(denominatorSelect);
}

void TransportPanel::update() {
    tempoText.setText(String(desktopController.getProject().getTempo()), dontSendNotification);
    numeratorText.setText(
        String(desktopController.getProject().getTimeSignature().getNumerator()), dontSendNotification);
    denominatorSelect.setSelectedId(
        desktopController.getProject().getTimeSignature().getDenominator(), dontSendNotification);
}

void TransportPanel::selectionChanged(Track *track) { transportControl.selectionChanged(track); }

void TransportPanel::paint(Graphics &g) {
    g.fillAll(Colours::lightgrey);
    auto area = getLocalBounds();
    g.setColour(Colours::dimgrey);
    g.fillRect(area.removeFromTop(1));
    g.fillRect(area.removeFromBottom(1));
}

void TransportPanel::resized() {
    auto area = getLocalBounds().withTrimmedTop(1).withTrimmedBottom(1);
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
