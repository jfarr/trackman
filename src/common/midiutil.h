#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

static int getLowestNote(const MidiMessageSequence &messages) {
    int note = INT_MAX;
    for (auto i : messages) {
        if (i->message.isNoteOn()) {
            note = std::min(note, i->message.getNoteNumber());
        }
    }
    return note == INT_MAX ? 0 : note;
}

static int getHighestNote(const MidiMessageSequence &messages) {
    int note = 0;
    for (auto i : messages) {
        if (i->message.isNoteOn()) {
            note = std::max(note, i->message.getNoteNumber());
        }
    }
    return note;
}

} // namespace trackman
