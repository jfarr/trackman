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
    setSize(getTrackWidth(), 280);
    for (TrackControl *lane : tracks) {
        lane->resized();
    }
    resized();
}

void TrackPanel::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
}

int TrackPanel::getTrackWidth() const { return std::max((int)(100 * tracks.size()), viewport.getWidth()); }

void TrackPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackPanel::resized() {
    auto area = getLocalBounds();
    for (auto &track : tracks) {
        track->setBounds(area.removeFromLeft(track->getPreferredWidth()));
    }
}
