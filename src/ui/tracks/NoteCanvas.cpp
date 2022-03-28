#include "NoteCanvas.h"
#include "common/midiutil.h"

namespace trackman {

NoteCanvas::NoteCanvas(Project &project, Track &track, NoteRoll &noteRoll)
    : project(project), track(track), noteRoll(noteRoll), dragImage(Image::ARGB, 1, 1, true),
      scaledDragImage(dragImage) {
    setSize(200, 81);
    startTimer(20);
}

void NoteCanvas::resize() {
    auto area = getBounds();
    auto pos = project.getTransport().getTransportSource().getCurrentPosition();
    auto length = project.ticksToSeconds(track.getCurrentMidiEndTimeInTicks(noteRoll, pos));
    auto w = noteRoll.empty() ? 0 : length * project.getHorizontalScale() + 1;
    setBounds(area.withWidth(w));
    repaint();
}

void NoteCanvas::paint(Graphics &g) {
    auto area = getLocalBounds();
    auto selected = track.isSelected() && noteRoll.isSelected();
    auto selectionBorder = 2;

    auto bgcolor = selected ? Colours::lightgrey : Colours::dimgrey;
    g.fillAll(bgcolor);
    g.setColour(Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(selectionBorder);
    g.setColour(Colours::dimgrey);
    g.fillRect(thumbnailBounds);

    auto scale = project.getHorizontalScale();
    int lowNote = noteRoll.getLowestNote();
    int highNote = noteRoll.getHighestNote();
    double noteSpan = max(24, highNote - lowNote) + 1;
    double x = 0;
    double h = getHeight();
    double noteHeight = h / noteSpan;
    auto pos = project.getTransport().getTransportSource().getCurrentPosition();
    track.eachCurrentMidiMessage(noteRoll, pos,
        [this, &g, lowNote, noteHeight, x, h, scale](const MidiMessageSequence::MidiEventHolder &eventHandle) {
            if (eventHandle.message.isNoteOn() && eventHandle.noteOffObject != nullptr) {
                auto noteOn = eventHandle.message;
                auto noteOff = eventHandle.noteOffObject->message;
                Rectangle<float> r = getNoteRect(noteOn, noteOff, lowNote, noteHeight, x, h, scale);
                g.setColour(Colours::steelblue.brighter(0.2));
                g.fillRect(r);
            }
        });
}

Rectangle<float> NoteCanvas::getNoteRect(const MidiMessage &noteOn, const MidiMessage &noteOff, int lowNote,
    double noteHeight, double x, double h, double scale) {
    double margin = 1.0;
    auto start = project.ticksToSeconds(noteOn.getTimeStamp());
    auto end = project.ticksToSeconds(noteOff.getTimeStamp());
    auto noteX = x + start * scale;
    auto noteWidth = (end - start) * scale;
    auto noteDist = noteOn.getNoteNumber() - lowNote;
    double noteY = h - noteHeight * (noteDist + margin);
    return Rectangle<float>(noteX, noteY, noteWidth, noteHeight).expanded(margin);
}

void NoteCanvas::mouseDown(const MouseEvent &event) {
    Component::mouseDown(event);
    notifySelected(track, noteRoll);
    if (!dragging) {
        dragging = true;
        xPos = getPosition().getX();
    }
}

void NoteCanvas::mouseUp(const MouseEvent &event) { dragging = false; }

void NoteCanvas::mouseDrag(const MouseEvent &event) {
    auto *container = DragAndDropContainer::findParentDragContainerFor(this);
    if (container != nullptr) {
        container->startDragging("notes", this, scaledDragImage);
    }
    auto d = event.getDistanceFromDragStartX();
    notifyMouseDragged(*this, xPos + d, event.getScreenY());
}

void NoteCanvas::notifySelected(Track &track, NoteRoll &selected) const {
    if (onSelected != nullptr) {
        onSelected(track, selected);
    }
}

void NoteCanvas::notifyMouseDragged(NoteCanvas &canvas, int x, int screenY) const {
    if (onMouseDragged != nullptr) {
        onMouseDragged(canvas, x, screenY);
    }
}

} // namespace trackman
