#include "InstrumentControl.h"

void InstrumentControl::update() {}

void InstrumentControl::paint(juce::Graphics &g) {
    g.fillAll(track.isSelected() ? juce::Colours::lightgrey : juce::Colours::grey);
    g.setColour(track.isSelected() ? juce::Colours::slategrey : juce::Colours::darkslategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void InstrumentControl::resized() {}
