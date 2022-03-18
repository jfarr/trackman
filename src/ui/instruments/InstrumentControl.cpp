#include "InstrumentControl.h"

InstrumentControl::InstrumentControl(Track &track) : track(track) {
    selectButton.setButtonText("...");
    selectButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkslategrey);
    addAndMakeVisible(selectButton);
}

void InstrumentControl::update() {}

void InstrumentControl::paint(juce::Graphics &g) {
    g.fillAll(track.isSelected() ? juce::Colours::lightgrey : juce::Colours::grey);
    g.setColour(track.isSelected() ? juce::Colours::slategrey : juce::Colours::darkslategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
}

void InstrumentControl::resized() {
    auto area = getLocalBounds();
    auto buttonSize = 25;
    auto margin = 3;
    auto buttonArea = area.removeFromLeft(buttonSize);
    selectButton.setBounds(buttonArea.removeFromTop(buttonSize).reduced(margin));
}
