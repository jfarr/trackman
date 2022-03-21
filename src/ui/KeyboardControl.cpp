#include "KeyboardControl.h"

namespace trackman {

KeyboardControl::KeyboardControl(MidiKeyboardState &keyboardState)
    : keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard) {

    setOpaque(true);
    addAndMakeVisible(keyboardComponent);

    setSize(450, 100);
}

void KeyboardControl::paint(Graphics &g) { g.fillAll(Colours::black); }

void KeyboardControl::resized() {
    auto area = getLocalBounds();
    keyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
}

} // namespace trackman
