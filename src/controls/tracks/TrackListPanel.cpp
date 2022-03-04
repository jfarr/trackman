#include "TrackListPanel.h"

TrackListPanel::TrackListPanel(TrackList &trackList) : trackList(trackList) { setSize(1200, 0); }

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::update() {
    removeAllChildren();
    for (TrackLaneControl *lane : lanes) {
        addAndMakeVisible(lane);
    }
    if (!lanes.empty()) {
        setSize(getWidth(), lanes.size() * lanes.back()->getHeight());
    }
    resized();
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(lane->getHeight()));
    }
}
