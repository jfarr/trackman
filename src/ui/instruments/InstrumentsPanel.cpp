#include "InstrumentsPanel.h"

void InstrumentsPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void InstrumentsPanel::resized() {
}
