#include "MixerPanel.h"
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
    mixerViewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
    addAndMakeVisible(mixerViewport);
}

//==============================================================================
void MixerPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    auto area = getLocalBounds();
//    auto buttonHeight = 35;
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(area.removeFromTop(transportHeight));
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(area.removeFromTop(1));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
//    auto buttonHeight = 35;
//    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(transportHeight).reduced(transportMargin));
    area.removeFromTop(1);
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getPreferredWidth()));
    mixerViewport.setBounds(area);
}
