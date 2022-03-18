#include "KeyboardControl.h"

KeyboardControl::KeyboardControl(juce::MidiKeyboardState &keyboardState)
    : keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard) {

    setOpaque(true);
    addAndMakeVisible(keyboardComponent);

    setSize(450, 100);
}

void KeyboardControl::paint(juce::Graphics &g) { g.fillAll(juce::Colours::black); }

void KeyboardControl::resized() {
    auto area = getLocalBounds();
    keyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
}
