#include "TrackLaneControl.h"

TrackLaneControl::TrackLaneControl() {
    createControls();
    setSize(0, 100);
}

TrackLaneControl::~TrackLaneControl() {}

void TrackLaneControl::createControls() {}

void TrackLaneControl::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::darkslategrey);
    g.setColour(juce::Colours::slategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void TrackLaneControl::resized() { auto area = getLocalBounds(); }
