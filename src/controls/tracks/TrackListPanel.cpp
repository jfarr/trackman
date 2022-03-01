#include "TrackListPanel.h"

TrackListPanel::TrackListPanel() { setSize(1200, 0); }

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::addTrack(TrackLaneControl& lane) {
    lanes.push_back(&lane);
    addAndMakeVisible(lane);
    setSize(getWidth(), lanes.size() * lanes.back()->getHeight());
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    for (auto &lane : lanes)
        lane->setBounds(area.removeFromTop(lane->getHeight()));
}
