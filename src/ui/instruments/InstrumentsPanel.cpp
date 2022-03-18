#include "InstrumentsPanel.h"

void InstrumentsPanel::update() {
    removeAllChildren();
    for (InstrumentControl *instrument : instruments) {
        instrument->update();
        addAndMakeVisible(instrument);
    }
    resize();
}

void InstrumentsPanel::resize() {
    setSize(panelWidth,
        topStripHeight + (instruments.empty() ? 0 : instruments.size() * instruments.back()->getPreferredHeight()));
    for (auto *instrument : instruments) {
        instrument->resized();
    }
    resized();
}

void InstrumentsPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void InstrumentsPanel::resized() {
    auto area = getLocalBounds();
    auto rightMargin = 3;
    area.removeFromTop(topStripHeight);
    area.removeFromRight(rightMargin);
    for (auto &instrument : instruments) {
        instrument->setBounds(area.removeFromTop(instrument->getPreferredHeight()));
    }
}
