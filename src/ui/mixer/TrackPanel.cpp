#include "TrackPanel.h"

TrackPanel::TrackPanel(juce::Viewport &viewport) : viewport(viewport) {}

void TrackPanel::update() {
    removeAllChildren();
    for (TrackControl *track : tracks) {
        track->update();
        addAndMakeVisible(track);
    }
    resize();
}

void TrackPanel::resize() {
    auto w = getTrackWidth();
    auto h = 244 - (w > viewport.getMaximumVisibleWidth() ? viewport.getScrollBarThickness() : 0);
    setSize(getTrackWidth(), h);
    for (TrackControl *track : tracks) {
        track->resized();
    }
    resized();
}

void TrackPanel::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
}

int TrackPanel::getTrackWidth() const { return TrackControl::getPreferredWidth() * tracks.size(); }

void TrackPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackPanel::resized() {
    auto area = getLocalBounds();
    for (auto &track : tracks) {
        track->setBounds(area.removeFromLeft(track->getPreferredWidth()));
    }
}
