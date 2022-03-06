#include "TimeMeter.h"

void TimeMeter::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds();
    auto leftPanelWidth = 25;
    auto labelWidth = 50;
    auto labelHeight = 20;
    g.fillAll(juce::Colours::grey);
    g.setColour(juce::Colour{0xff282828});
    int i = 1;
    while (i * scale < bounds.getWidth()) {
        g.drawText(juce::String(i) + ":00", leftPanelWidth+ i * scale - labelWidth / 2, 0, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::horizontallyCentred, false);
        i++;
    }
}
