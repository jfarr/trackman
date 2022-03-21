#include "MixerPanel.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

MixerPanel::MixerPanel(DesktopController &desktopController, LevelMeterSource &meterSource)
    : desktopController(desktopController), masterTrackControl(desktopController.getMixer(), meterSource),
      mixerViewport(desktopController.getMixerController().getViewport()) {

    createControls();
    update();
    masterTrackControl.addListener(&desktopController);
    setSize(preferredWidth, preferredHeight);
}

MixerPanel::~MixerPanel() {
    masterTrackControl.removeListener(&desktopController);
}

void MixerPanel::createControls() {
    mixerViewport.getHorizontalScrollBar().setColour(ScrollBar::thumbColourId, Colours::dimgrey);
    addAndMakeVisible(masterTrackControl);
    addAndMakeVisible(mixerViewport);
}

void MixerPanel::update() {
}

//==============================================================================
void MixerPanel::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getPreferredWidth()));
    mixerViewport.setBounds(area);
}

} // namespace trackman
