#include "KeyboardControl.h"

KeyboardControl::KeyboardControl()
    : keyboardComponent(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard),
      startTime(juce::Time::getMillisecondCounterHiRes() * 0.001) {

    setOpaque(true);
    addAndMakeVisible(keyboardComponent);
    keyboardState.addListener(this);

    addAndMakeVisible(midiMessagesBox);
    midiMessagesBox.setMultiLine(true);
    midiMessagesBox.setReturnKeyStartsNewLine(true);
    midiMessagesBox.setReadOnly(true);
    midiMessagesBox.setScrollbarsShown(true);
    midiMessagesBox.setCaretVisible(false);
    midiMessagesBox.setPopupMenuEnabled(true);
    midiMessagesBox.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0x32ffffff));
    midiMessagesBox.setColour(juce::TextEditor::outlineColourId, juce::Colour(0x1c000000));
    midiMessagesBox.setColour(juce::TextEditor::shadowColourId, juce::Colour(0x16000000));

    setSize(450, 200);
}

KeyboardControl::~KeyboardControl() { keyboardState.removeListener(this); }

void KeyboardControl::paint(juce::Graphics &g) { g.fillAll(juce::Colours::black); }

void KeyboardControl::resized() {
    auto area = getLocalBounds();
    keyboardComponent.setBounds(area.removeFromTop(80).reduced(8));
    midiMessagesBox.setBounds(area.reduced(8));
}

void KeyboardControl::handleNoteOn(
    juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    postMessageToList(m, "On-Screen Keyboard");
}

void KeyboardControl::handleNoteOff(
    juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) {
    auto m = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber);
    m.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);
    postMessageToList(m, "On-Screen Keyboard");
}

void KeyboardControl::postMessageToList(const juce::MidiMessage &message, const juce::String &source) {
    (new IncomingMessageCallback(this, message, source))->post();
}

void KeyboardControl::addMessageToList(const juce::MidiMessage &message, const juce::String &source) {
    auto time = message.getTimeStamp() - startTime;

    auto hours = ((int)(time / 3600.0)) % 24;
    auto minutes = ((int)(time / 60.0)) % 60;
    auto seconds = ((int)time) % 60;
    auto millis = ((int)(time * 1000.0)) % 1000;

    auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d", hours, minutes, seconds, millis);

    auto description = getMidiMessageDescription(message);

    juce::String midiMessageString(timecode + "  -  " + description + " (" + source + ")"); // [7]
    logMessage(midiMessageString);
}
