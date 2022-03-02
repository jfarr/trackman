#include "TrackLaneControl.h"

TrackLaneControl::TrackLaneControl() {
    createControls();
    setSize(800, 100);
}

TrackLaneControl::~TrackLaneControl() {}

void TrackLaneControl::createControls() {}

void TrackLaneControl::setSelected(bool newSelected) {
    selected = newSelected;
    repaint();
}

void TrackLaneControl::paint(juce::Graphics &g) {
    g.fillAll(selected ? juce::Colours::lightseagreen : juce::Colours::darkslategrey);
    g.setColour(juce::Colours::slategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void TrackLaneControl::resized() {
    // auto area = getLocalBounds();
}
