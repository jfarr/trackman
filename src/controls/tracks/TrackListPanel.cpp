#include "TrackListPanel.h"

TrackListPanel::TrackListPanel(TrackList &trackList, juce::Viewport &viewport)
    : trackList(trackList), viewport(viewport) {
//    setSize(getTrackLaneWidth(), 0);
}

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::update() {
    removeAllChildren();
    for (TrackLaneControl *lane : lanes) {
        addAndMakeVisible(lane);
    }
    resize();
}

void TrackListPanel::resize() {
    if (!lanes.empty()) {
        setSize(getTrackLaneWidth(), lanes.size() * lanes.back()->getHeight());
    }
    resized();
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto area = getLocalBounds().withWidth(getTrackLaneWidth());
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(lane->getHeight()));
    }
}

int TrackListPanel::getTrackLaneWidth() const {
    auto trackWidth = (int)(trackList.getTotalLength() * scale);
    return std::max(trackWidth, viewport.getWidth());
}
