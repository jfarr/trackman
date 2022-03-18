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
}
