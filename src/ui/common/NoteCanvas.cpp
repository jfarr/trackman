#include "NoteCanvas.h"

NoteCanvas::NoteCanvas(Project &project, Track &track) : project(project), track(track) {
    setInterceptsMouseClicks(false, false);
    setSize(200, 81);
    startTimer(20);
}

void NoteCanvas::resize() {
    auto area = getBounds();
    auto length =
        track.getCurrentMidiMessages(project.getMixer().getTransportSource().getCurrentPosition()).getEndTime();
    auto width = length * project.getHorizontalScale();
    setBounds(area.withWidth(width));
    repaint();
}

void NoteCanvas::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::green);
    auto selected = track.isSelected();
    auto area = getLocalBounds();
    auto messages = track.getCurrentMidiMessages(project.getMixer().getTransportSource().getCurrentPosition());
    auto length = messages.getEndTime();
    auto scale = area.getWidth() / length;
    for (auto m : messages) {
        if (m->message.isNoteOn() && m->noteOffObject != nullptr) {
            auto noteOn = m->message;
            auto noteOff = m->noteOffObject->message;
            auto start = noteOn.getTimeStamp();
            auto end = noteOff.getTimeStamp();
            auto x = area.getX() + start * scale;
            auto width = (end - start) * scale;
            juce::Rectangle<int> r(x, 10, width, 5);
            g.setColour(juce::Colours::steelblue.darker(0.3));
            g.fillRect(r);
            g.setColour(selected ? juce::Colours::lightgrey : juce::Colours::grey);
            g.drawRect(r.expanded(1));
        }
    }
}
