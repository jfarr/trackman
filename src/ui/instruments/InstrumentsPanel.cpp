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
    double paneHeight = getPaneHeight();
    setSize(panelWidth, topStripHeight + instruments.size() * paneHeight);
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
        instrument->setBounds(area.removeFromTop(getPaneHeight()));
    }
}

double InstrumentsPanel::getPaneHeight() {
    auto paneHeight = instruments.empty() ? 0 : instruments.back()->getPreferredHeight() * project.getVerticalScale();
    return paneHeight;
}
