#include "MixerPanel.h"
#include "controls/desktop/DesktopController.h"

MixerPanel::MixerPanel(
    DesktopController &desktopController, TrackList &trackList, Mixer &mixer, foleys::LevelMeterSource &meterSource)
    : desktopController(desktopController), trackList(trackList), mixer(mixer), masterTrackControl(mixer, meterSource),
      mixerViewport(desktopController.getMixerController().getViewport()),
      transportControl(mixer.getTransportSource()) {
    createControls();
    masterTrackControl.addListener(&desktopController);
    setSize(800, 280);
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
    auto buttonHeight = 35;
    g.setColour(juce::Colours::lightgrey);
    g.fillRect(area.removeFromTop(buttonHeight));
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(area.removeFromTop(1));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
    auto buttonHeight = 35;
    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(buttonHeight).reduced(transportMargin));
    area.removeFromTop(1);
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getPreferredWidth()));
    mixerViewport.setBounds(area);
}
