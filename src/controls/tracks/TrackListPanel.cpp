#include "TrackListPanel.h"

TrackListPanel::TrackListPanel() { setSize(1200, 0); }

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::addTrack(Track &track) {
    tracks.push_back(&track);
    addAndMakeVisible(track.getTrackLaneControl());
    setSize(getWidth(), tracks.size() * tracks.back()->getTrackLaneControl().getHeight());
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    for (auto &track : tracks)
        track->getTrackLaneControl().setBounds(area.removeFromTop(track->getTrackLaneControl().getHeight()));
}
