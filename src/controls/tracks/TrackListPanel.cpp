#include "TrackListPanel.h"

TrackListPanel::TrackListPanel(TrackList &trackList, juce::Viewport &viewport)
    : trackList(trackList), viewport(viewport) {}

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::update() {
    removeAllChildren();
    for (TrackLaneControl *lane : lanes) {
        addAndMakeVisible(lane);
    }
    resize();
}

void TrackListPanel::fileDragEnter(const juce::StringArray &files, int x, int y) {
    addAndMakeVisible(dropBox);
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withCentre(juce::Point(x, y)));
}

void TrackListPanel::fileDragMove(const juce::StringArray &files, int x, int y) {
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withCentre(juce::Point(x, y)));
}

void TrackListPanel::fileDragExit(const juce::StringArray &files) { removeChildComponent(&dropBox); }

void TrackListPanel::filesDropped(const juce::StringArray &files, int x, int y) { removeChildComponent(&dropBox); }

Track *TrackListPanel::getTrackAtPos(int x, int y) {
    for (TrackLaneControl *lane : lanes) {
        auto area = lane->getBoundsInParent();
        if (area.contains(x, y)) {
            return &lane->getTrack();
        }
    }
    return nullptr;
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
