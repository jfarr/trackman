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
    auto border = 3;
    auto pos = project.getTransport().getTransportSource().getCurrentPosition();
    auto length = project.ticksToSeconds(track.getCurrentMidiEndTimeInTicks(noteRoll, pos));
    auto w = noteRoll.empty() ? 0 : length * project.getHorizontalScale() + 2 * border + 1;
    setBounds(area.withWidth(w));
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

    auto scale = project.getHorizontalScale();
    int lowNote = noteRoll.getLowestNote();
    int highNote = noteRoll.getHighestNote();
    double noteSpan = max(24, highNote - lowNote) + 1;
    double margin = 1.0;
    double x = border;
    double h = getHeight() - 2 * border;
    double noteHeight = h / noteSpan;
    auto pos = project.getTransport().getTransportSource().getCurrentPosition();
    track.eachCurrentMidiMessage(noteRoll, pos,
        [this, &g, lowNote, noteHeight, x, h, scale, border, margin, selected](
            const MidiMessageSequence::MidiEventHolder &eventHandle) {
            if (eventHandle.message.isNoteOn() && eventHandle.noteOffObject != nullptr) {
                auto noteOn = eventHandle.message;
                auto noteOff = eventHandle.noteOffObject->message;
                Rectangle<float> r = getNoteRect(noteOn, noteOff, lowNote, noteHeight, x, h, scale);
                r.setY(r.getY() + border);
                g.setColour(Colours::steelblue.darker(0.3));
                g.fillRect(r);
                g.setColour(selected ? Colours::lightgrey : Colours::grey);
                g.drawRect(r.expanded(margin));
            }
        });
}

Rectangle<float> NoteCanvas::getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
    double noteHeight, double x, double h, double scale) {
    auto start = project.ticksToSeconds(noteOn.getTimeStamp());
    auto end = project.ticksToSeconds(noteOff.getTimeStamp());
    auto noteX = x + start * scale;
    auto noteWidth = (end - start) * scale;
    auto noteDist = noteOn.getNoteNumber() - lowNote;
    double noteY = h - noteHeight * (noteDist + 1.0);
    return Rectangle<float>(noteX, noteY, noteWidth, noteHeight);
}

} // namespace trackman
