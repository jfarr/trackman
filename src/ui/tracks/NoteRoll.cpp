#include "NoteRoll.h"
#include "common/midiutil.h"

NoteRoll::NoteRoll(Project &project, Track &track) : project(project), track(track) {
    setInterceptsMouseClicks(false, false);
    setSize(200, 81);
    startTimer(20);
}

void NoteRoll::resize() {
    auto area = getBounds();
    auto length = project.ticksToSeconds(
        track.getCurrentMidiMessages(project.getMixer().getTransportSource().getCurrentPosition()).getEndTime());
    auto width = length * project.getHorizontalScale() + 1;
    setBounds(area.withWidth(width));
    repaint();
}

void NoteRoll::paint(juce::Graphics &g) {
    auto selected = track.isSelected();
    auto area = getLocalBounds();
    auto messages = track.getCurrentMidiMessages(project.getMixer().getTransportSource().getCurrentPosition());
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
            juce::Rectangle<float> r = getNoteRect(noteOn, noteOff, lowNote, noteHeight, x, h, scale, margin);
            g.setColour(juce::Colours::steelblue.darker(0.3));
            g.fillRect(r);
            g.setColour(selected ? juce::Colours::lightgrey : juce::Colours::grey);
            g.drawRect(r.expanded(margin));
        }
    }
}

juce::Rectangle<float> NoteRoll::getNoteRect(const juce::MidiMessage &noteOn, const juce::MidiMessage &noteOff,
    int lowNote, double noteHeight, double x, double h, double scale, double margin) {
    auto start = project.ticksToSeconds(noteOn.getTimeStamp());
    auto end = project.ticksToSeconds(noteOff.getTimeStamp());
    auto noteX = x + start * scale;
    auto noteWidth = (end - start) * scale;
    auto noteDist = noteOn.getNoteNumber() - lowNote;
    double noteY = h - noteHeight * (noteDist + 1.0) + margin;
    return juce::Rectangle<float>(noteX, noteY, noteWidth, noteHeight);
}
