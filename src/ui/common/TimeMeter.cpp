#include "TimeMeter.h"

void TimeMeter::paint(juce::Graphics &g) {
    g.fillAll(juce::Colours::grey);
    auto bounds = getLocalBounds();
    drawTicksInMeasures(g, bounds.removeFromTop(15));
    drawTicksInSeconds(g, bounds);
    drawStartMarker(g);
}

void TimeMeter::drawTicksInMeasures(juce::Graphics &g, const juce::Rectangle<int> &bounds) const {
    auto labelY = bounds.getY() - 7;
    auto labelWidth = 75;
    auto labelHeight = 20;
    auto labelMargin = 5;
    float tickHeight = 15;
    float scale = project.getHorizontalScale();
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    float y = bounds.getY() + (bounds.getHeight() - tickHeight);
    //    float i = 0;
    double secs = 0;
    for (int i = 0; secs * scale < bounds.getWidth(); i++, secs = project.measuresToSeconds(i)) {
        DBG("i: " << i << " secs: " << secs);
        //    double increment = project.measuresToSeconds(2) * project.getHorizontalScaleRatio();
        //    if (increment <= 0) {
        //        return;
        //    }
        //    int n = 1;
        //    while (i * scale < bounds.getWidth()) {
        float x = secs * scale;
        g.drawRect(x, y, 1.0, tickHeight, 1.0);
        g.drawText(juce::String(i + 1) + ".1.00", x + labelMargin, labelY, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::left, true);
        //        n++;
        //        i += increment;
    }
}

void TimeMeter::drawTicksInSeconds(juce::Graphics &g, const juce::Rectangle<int> &bounds) const {
    auto labelY = bounds.getY() - 12;
    auto labelWidth = 75;
    auto labelHeight = 20;
    auto labelMargin = 5;
    float tickHeight = 15;
    float scale = project.getHorizontalScale();
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    float y = bounds.getY() + (bounds.getHeight() - tickHeight);
    //    float i = 0;
    double secs = 0;
    for (int i = 0; secs * scale < bounds.getWidth(); i++, secs = project.measuresToSeconds(i)) {
        DBG("i: " << i << " secs: " << secs);
        //    double increment = project.measuresToSeconds(2) * project.getHorizontalScaleRatio();
        //    if (increment <= 0) {
        //        return;
        //    }
        //    int n = 1;
        //    while (i * scale < bounds.getWidth()) {
        float x = secs * scale;
        float secs = project.measuresToSeconds(i);
        float wholeSecs;
        float decimalSecs = std::modf(secs, &wholeSecs);
        int mins = (int)(wholeSecs / 60);
        secs = std::fmod(secs, 60);

        g.drawRect(x, y, 1.0, tickHeight, 1.0);
        g.drawText(juce::String(mins).paddedLeft('0', 2) + ":" + juce::String(secs, 3).paddedLeft('0', 6),
            x + labelMargin, labelY, labelWidth, labelHeight, juce::Justification::bottom | juce::Justification::left,
            true);
        //        n++;
        //        i += increment;
    }

    //    float y = bounds.getY() + (bounds.getHeight() - tickHeight);
    //    float i = 0;
    //    double increment = project.measuresToSeconds(1) * project.getHorizontalScaleRatio();
    //    int n = 1;
    //    while (i * scale < bounds.getWidth()) {
    //        float x = i * scale;
    //        g.drawRect(x, y, 1.0, tickHeight, 1.0);
    //        float secs = project.measuresToSeconds(n);
    //        float wholeSecs;
    //        float decimalSecs = std::modf(secs, &wholeSecs);
    //        int mins = (int)(wholeSecs / 60);
    //        secs = std::fmod(secs, 60);
    ////        int fraction = (int)std::round(decimalSecs * 1000) / 1000;
    //        g.drawText(juce::String(mins).paddedLeft('0', 2) + ":" + juce::String(secs, 3).paddedLeft('0', 6),
    ////        g.drawText(juce::String(mins).paddedLeft('0', 2) + ":" + juce::String((int)wholeSecs).paddedLeft('0', 2)
    ///+ "." + /                       juce::String(fraction, 3),
    //            x + labelMargin, labelY, labelWidth, labelHeight, juce::Justification::bottom |
    //            juce::Justification::left, true);
    //        n++;
    //        i += increment;
    //    }

    //    auto labelY = bounds.getY() - 7;
    //    auto labelWidth = 50;
    //    auto labelHeight = 20;
    //    auto labelMargin = 5;
    //    float tickHeight = 15;
    //    float secondTickHeight = 5;
    //    float thirdTickHeight = 3;
    //    float scale = project.getHorizontalScale();
    //    g.setColour(juce::Colour{0xff282828});
    //    g.setFont(11);
    //    float y = bounds.getY() + (bounds.getHeight() - tickHeight);
    //    float i = 0;
    //    float increment = std::max(1.0, project.getHorizontalScaleRatio());
    //    while (i * scale < bounds.getWidth()) {
    //        float x = i * scale;
    //        g.drawRect(x, y, 1.0, tickHeight, 1.0);
    //        g.drawText(juce::String(i) + ":00", x + labelMargin, labelY, labelWidth, labelHeight,
    //            juce::Justification::bottom | juce::Justification::left, true);
    //        i += increment;
    //    }
    //    y = getHeight() - secondTickHeight;
    //    i = 1;
    //    while (i * scale / 2 * increment < bounds.getWidth()) {
    //        float x = i * scale / 2 * increment;
    //        g.drawRect(x, y, 1.0, secondTickHeight, 1.0);
    //        i += 1;
    //    }
    //    y = getHeight() - thirdTickHeight;
    //    i = 1;
    //    while (i * scale / 8 * increment < bounds.getWidth()) {
    //        float x = i * scale / 8 * increment;
    //        g.drawRect(x, y, 1.0, thirdTickHeight, 1.0);
    //        i += 1;
    //    }
}

void TimeMeter::drawStartMarker(juce::Graphics &g) const {
    auto margin = 4;
    auto arrowHeight = 8.0;
    auto arrowWidth = 18.0;

    float audioPosition = 0.0;
    auto drawPosition = audioPosition * project.getHorizontalScale();

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
