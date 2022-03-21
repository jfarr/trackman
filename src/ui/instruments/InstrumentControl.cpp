#include "InstrumentControl.h"

namespace trackman {

InstrumentControl::InstrumentControl(Track &track) : track(track) {
    selectButton.setButtonText("...");
    selectButton.setColour(TextButton::buttonColourId, Colours::darkslategrey);
    addAndMakeVisible(selectButton);
}

void InstrumentControl::update() { selectButton.setEnabled(track.canRecord()); }

void InstrumentControl::paint(Graphics &g) {
    g.fillAll(track.isSelected() ? Colours::lightgrey : Colours::grey);
    g.setColour(track.isSelected() ? Colours::slategrey : Colours::darkslategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void InstrumentControl::resized() {
    auto area = getLocalBounds();
    auto buttonSize = 25;
    auto margin = 3;
    auto buttonArea = area.removeFromLeft(buttonSize);
    selectButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}

} // namespace trackman
