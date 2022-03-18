#include "NoteCanvas.h"

NoteCanvas::NoteCanvas(Project &project, Track &track)
    : project(project), track(track) {
    setSize(200, 81);
}

void NoteCanvas::resize() {
    auto length = track.getMidiMessages().getEndTime() - track.getMidiMessages().getStartTime();
    auto width = length * project.getHorizontalScale();
    setSize(width, getHeight());
    repaint();
}

void NoteCanvas::paint(juce::Graphics &g) {
    auto area = getLocalBounds();
    auto margin = 3;

    auto bgcolor = juce::Colours::dimgrey;
    g.fillAll(bgcolor);
    g.setColour(juce::Colours::grey);
    g.drawRect(0, 0, getWidth(), getHeight());

    auto thumbnailBounds = area.reduced(margin);
    g.setColour(juce::Colours::dimgrey);
    g.fillRect(thumbnailBounds);

    auto length = track.getMidiMessages().getEndTime(); // - track.getMidiMessages().getStartTime();
    auto scale = area.getWidth() / length;
    for (auto m : track.getMidiMessages()) {
        if (m->message.isNoteOn() && m->noteOffObject != nullptr) {
            auto start = m->message.getTimeStamp();
            auto end = m->noteOffObject->message.getTimeStamp();
            auto x = start * scale;
            auto width = (end - start) * scale;
            juce::Rectangle<int> r(x, 10, width, 5);
            g.setColour(juce::Colours::steelblue.darker(0.3));
            g.fillRect(r);
            g.setColour(juce::Colours::lightgrey);
            g.drawRect(r.expanded(1));
        }
    }
}
