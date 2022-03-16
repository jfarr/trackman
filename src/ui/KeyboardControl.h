#pragma once

#include "JuceHeader.h"

class KeyboardControl : public juce::Component, public juce::MidiKeyboardStateListener {
  public:
    KeyboardControl();
    ~KeyboardControl() override;

    //==============================================================================
    // Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // MidiKeyboardStateListener
    void handleNoteOn(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(juce::MidiKeyboardState *source, int midiChannel, int midiNoteNumber, float velocity) override;

  private:
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;
    juce::TextEditor midiMessagesBox;
    double startTime;

    // This is used to dispach an incoming message to the message thread
    class IncomingMessageCallback : public juce::CallbackMessage {
      public:
        IncomingMessageCallback(KeyboardControl *o, const juce::MidiMessage &m, const juce::String &s)
            : owner(o), message(m), source(s) {}

        void messageCallback() override {
            if (owner != nullptr)
                owner->addMessageToList(message, source);
        }

        Component::SafePointer<KeyboardControl> owner;
        juce::MidiMessage message;
        juce::String source;
    };

    void postMessageToList(const juce::MidiMessage &message, const juce::String &source);
    void addMessageToList(const juce::MidiMessage &message, const juce::String &source);

    static juce::String getMidiMessageDescription(const juce::MidiMessage &m) {
        if (m.isNoteOn())
            return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
        if (m.isNoteOff())
            return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
        if (m.isProgramChange())
            return "Program change " + juce::String(m.getProgramChangeNumber());
        if (m.isPitchWheel())
            return "Pitch wheel " + juce::String(m.getPitchWheelValue());
        if (m.isAftertouch())
            return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " +
                   juce::String(m.getAfterTouchValue());
        if (m.isChannelPressure())
            return "Channel pressure " + juce::String(m.getChannelPressureValue());
        if (m.isAllNotesOff())
            return "All notes off";
        if (m.isAllSoundOff())
            return "All sound off";
        if (m.isMetaEvent())
            return "Meta event";

        if (m.isController()) {
            juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

            if (name.isEmpty())
                name = "[" + juce::String(m.getControllerNumber()) + "]";

            return "Controller " + name + ": " + juce::String(m.getControllerValue());
        }

        return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
    }

    void logMessage(const juce::String &m) {
        midiMessagesBox.moveCaretToEnd();
        midiMessagesBox.insertTextAtCaret(m + juce::newLine);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KeyboardControl)
};
