#include "TimeMeter.h"

void TimeMeter::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds();
    auto leftPanelWidth = 25;
    auto labelY = -7;
    auto labelWidth = 50;
    auto labelHeight = 20;
    auto labelMargin = 5;
    auto tickHeight = 15;
    auto secondTickHeight = 5;
    auto thirdTickHeight = 3;
    g.fillAll(juce::Colours::grey);
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    int y = getHeight() - tickHeight;
    int i = 0;
    while (i * scale < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale;
        g.drawRect(x, y, 1, tickHeight, 1);
        g.drawText(juce::String(i) + ":00", x + labelMargin, labelY, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::left, true);
        i++;
    }
    y = getHeight() - secondTickHeight;
    i = 1;
    while (i * scale / 2 < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale / 2;
        g.drawRect(x, y, 1, secondTickHeight, 1);
        i++;
    }
    y = getHeight() - thirdTickHeight;
    i = 1;
    while (i * scale / 8 < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale / 8;
        g.drawRect(x, y, 1, thirdTickHeight, 1);
        i++;
    }
}
