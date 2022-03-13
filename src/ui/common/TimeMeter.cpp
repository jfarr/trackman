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
    auto scale = project.getHorizontalScale();
    g.fillAll(juce::Colours::grey);
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    int y = getHeight() - tickHeight;
    int i = 0;
    int increment = std::max(1, (int)(75 / project.getHorizontalScale()));
    while (i * scale < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale;
        g.drawRect(x, y, 1, tickHeight, 1);
        g.drawText(juce::String(i) + ":00", x + labelMargin, labelY, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::left, true);
        i += increment;
    }
    y = getHeight() - secondTickHeight;
    i = 1;
    while (i * scale / 2 * increment < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale / 2 * increment;
        g.drawRect(x, y, 1, secondTickHeight, 1);
        i += 1;
    }
    y = getHeight() - thirdTickHeight;
    i = 1;
    while (i * scale / 8 * increment < bounds.getWidth()) {
        int x = leftPanelWidth + i * scale / 8 * increment;
        g.drawRect(x, y, 1, thirdTickHeight, 1);
        i += 1;
    }

    auto margin = 4;
    auto arrowHeight = 8.0;
    auto arrowWidth = 18.0;

    float audioPosition = 0.0;
        auto drawPosition = audioPosition * scale + leftPanelWidth;

    g.setColour(juce::Colours::lightgrey);
    g.drawLine(drawPosition, margin, drawPosition, (float)getHeight(), 1.0f);

    auto a = juce::Point<float>(drawPosition - arrowWidth / 2, margin);
    auto b = juce::Point<float>(drawPosition + arrowWidth / 2, margin);
    auto c = juce::Point<float>(drawPosition, margin + arrowHeight);
    juce::Path path;
    path.addTriangle(a, b, c);
    g.setColour(juce::Colours::lightgrey);
    g.fillPath(path);

    arrowWidth *= 0.8;
    arrowHeight *= 0.8;
    margin += arrowHeight * 0.2;
    a = juce::Point<float>(drawPosition - arrowWidth / 2, margin);
    b = juce::Point<float>(drawPosition + arrowWidth / 2, margin);
    c = juce::Point<float>(drawPosition, margin + arrowHeight);
    juce::Path path2;
    path2.addTriangle(a, b, c);
    g.setColour(juce::Colours::steelblue);
    g.fillPath(path2);
}
