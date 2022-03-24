#include "NoteCanvas.h"
#include "common/midiutil.h"

namespace trackman {

NoteCanvas::NoteCanvas(Project &project, Track &track, NoteRoll &noteRoll)
    : project(project), track(track), noteRoll(noteRoll) {
    setInterceptsMouseClicks(false, false);
    setSize(200, 81);
    startTimer(20);
}

void NoteCanvas::resize() {
    auto area = getBounds();
    auto length = project.ticksToSeconds(
        track.getCurrentMidiMessages(project.getTransport().getTransportSource().getCurrentPosition()).getEndTime());
    auto width = length * project.getHorizontalScale() + 1;
    setBounds(area.withWidth(width));
    repaint();
}

void NoteCanvas::paint(Graphics &g) {
    auto area = getLocalBounds();
    auto selected = track.isSelected();
    auto border = 3;

    auto bgcolor = selected ? Colours::lightgrey : Colours::dimgrey;
    g.fillAll(bgcolor);
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(border);
    g.setColour(Colours::dimgrey);
    g.fillRect(thumbnailBounds);

    auto messages = track.getCurrentMidiMessages(project.getTransport().getTransportSource().getCurrentPosition());
    auto scale = project.getHorizontalScale();
    int lowNote = getLowestNote(messages);
    int highNote = getHighestNote(messages);
    double noteSpan = max(24, highNote - lowNote) + 1;
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

Rectangle<float> NoteCanvas::getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
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
