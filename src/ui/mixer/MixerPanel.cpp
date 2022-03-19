#include "MixerPanel.h"
#include "common/listutil.h"
#include "ui/desktop/DesktopController.h"

MixerPanel::MixerPanel(DesktopController &desktopController, foleys::LevelMeterSource &meterSource)
    : desktopController(desktopController), masterTrackControl(desktopController.getMixer(), meterSource),
      mixerViewport(desktopController.getMixerController().getViewport()),
      transportControl(desktopController.getMixer().getTransportSource(), true, &desktopController.getMidiRecorder(),
          &desktopController.getProject().getTrackList()) {
    createControls();
    masterTrackControl.addListener(&desktopController);
    setSize(preferredWidth, preferredHeight);
}

MixerPanel::~MixerPanel() { masterTrackControl.removeListener(&desktopController); }

void MixerPanel::createControls() {
    tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setJustificationType(juce::Justification::centredRight);
    tempoText.setText(juce::String(desktopController.getProject().getTempo()), juce::dontSendNotification);
    tempoText.setJustificationType(juce::Justification::centredLeft);
    tempoText.onTextChange = [this] { tempoChanged(); };
    mixerViewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    addAndMakeVisible(transportControl);
    addAndMakeVisible(tempoLabel);
    addAndMakeVisible(tempoText);
    addAndMakeVisible(masterTrackControl);
    addAndMakeVisible(mixerViewport);
}

void MixerPanel::update() {
    tempoText.setText(juce::String(desktopController.getProject().getTempo()), juce::dontSendNotification);
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
    transportControl.setBounds(area.removeFromTop(transportHeight).reduced(transportMargin));
    area.removeFromTop(1);
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getPreferredWidth()));
    mixerViewport.setBounds(area);
}

void MixerPanel::tempoChanged() {
    float newTempo = tempoText.getText().getFloatValue();;
    notifyTempoChanged(previousTempo, newTempo);
    previousTempo = newTempo;
}

void MixerPanel::addListener(MasterTrackListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void MixerPanel::removeListener(MasterTrackListener *listener) { listeners.remove(listener); }

void  MixerPanel::notifyTempoChanged(float previousTempo, float newTempo) {
    for (MasterTrackListener *listener : listeners) {
        listener->tempoChanged(previousTempo, newTempo);
    }

}
