#include "NoteRoll.h"
#include "common/midiutil.h"

namespace trackman {

NoteRoll::NoteRoll(Project &project, Track &track) : project(project), track(track) {
    setInterceptsMouseClicks(false, false);
    setSize(200, 81);
    startTimer(20);
}

void NoteRoll::resize() {
    auto area = getBounds();
    auto length = project.ticksToSeconds(
        track.getCurrentMidiMessages(project.getTransport().getTransportSource().getCurrentPosition()).getEndTime());
    auto width = length * project.getHorizontalScale() + 1;
    setBounds(area.withWidth(width));
    repaint();
}

void NoteRoll::paint(Graphics &g) {
    auto selected = track.isSelected();
    auto area = getLocalBounds();
    auto messages = track.getCurrentMidiMessages(project.getTransport().getTransportSource().getCurrentPosition());
    auto scale = project.getHorizontalScale();
    int lowNote = getLowestNote(messages);
    int highNote = getHighestNote(messages);
    double noteSpan = std::max(24, highNote - lowNote) + 1;
    double margin = 1.0;
    double x = area.getX();
    double h = getHeight() - 2 * margin;
    double noteHeight = h / noteSpan;
    for (auto m : messages) {
        if (m->message.isNoteOn() && m->noteOffObject != nullptr) {
            auto noteOn = m->message;
            auto noteOff = m->noteOffObject->message;
            Rectangle<float> r = getNoteRect(noteOn, noteOff, lowNote, noteHeight, x, h, scale, margin);
            g.setColour(Colours::steelblue.darker(0.3));
            g.fillRect(r);
            g.setColour(selected ? Colours::lightgrey : Colours::grey);
            g.drawRect(r.expanded(margin));
        }
    }
}

Rectangle<float> NoteRoll::getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
    double noteHeight, double x, double h, double scale, double margin) {
    auto start = project.ticksToSeconds(noteOn.getTimeStamp());
    auto end = project.ticksToSeconds(noteOff.getTimeStamp());
    auto noteX = x + start * scale;
    auto noteWidth = (end - start) * scale;
    auto noteDist = noteOn.getNoteNumber() - lowNote;
    double noteY = h - noteHeight * (noteDist + 1.0) + margin;
    return Rectangle<float>(noteX, noteY, noteWidth, noteHeight);
}

} // namespace trackman
