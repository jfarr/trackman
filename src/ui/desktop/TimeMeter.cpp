#include "TimeMeter.h"
#include "common/mathutil.h"

TimeMeter::TimeMeter(Project &project) : project(project) { setInterceptsMouseClicks(false, false); }

void TimeMeter::paint(juce::Graphics &g) {
    const int topStripHeight = 30;
    //    g.fillAll(juce::Colours::green);
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::grey);
    g.fillRect(0, 0, bounds.getWidth(), topStripHeight);
    auto scale = project.getHorizontalScale();
    const float numerator = project.getTimeSignature().getNumerator();
    const float denominator = project.getTimeSignature().getDenominator();
    const float dashes[]{0.5, 0.5};
    int increment = std::max(1, highestPowerOf2((int)(128.0 / scale))) * 2;
    DBG("increment: " << increment);
    for (int secs = increment, x = secs * scale * numerator / denominator; x < bounds.getWidth(); secs += increment, x = secs * scale * numerator / denominator) {
        auto line = juce::Line<float>(x, 0.0, x, bounds.getHeight());
        g.drawDashedLine(line, dashes, 2, 1.0);
    }

    //    int i = 0;
    //    auto denominator = project.getTimeSignature().getDenominator();
    //    int increment = 1;
    //    float x = project.measuresToSeconds(i) * project.getHorizontalScale();
    //    const float dashes[] { 0.5, 0.5 };
    //    for (; x < bounds.getWidth(); i += increment, x = project.measuresToSeconds(i) * project.getHorizontalScale())
    //    {
    ////        g.drawRect(x, 0.0, 1.0, (float)bounds.getHeight(), 1.0);
    //        auto line = juce::Line<float>(x, 0.0, x, bounds.getHeight());
    //        g.drawDashedLine(line, dashes, 2, 1.0);
    //    }

    //    drawTicksInMeasures(g, bounds.removeFromTop(15));
    //    drawTicksInSeconds(g, bounds.removeFromTop(15));
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
    double secs = 0;
    int increment = std::max(1, highestPowerOf2((int)(128.0 / scale)));
    bool drawHalfNotes = (float)(128.0 / scale) < 1.25;
    bool drawQuarterNotes = (float)(128.0 / scale) < 0.55;
    for (int i = 0; secs * scale < bounds.getWidth(); i += increment, secs = project.measuresToSeconds(i)) {
        float x = secs * scale;
        g.drawRect(x, y, 1.0, tickHeight, 1.0);
        //        g.drawText(juce::String(i + 1) + ".1.00", x + labelMargin, labelY, labelWidth, labelHeight,
        //            juce::Justification::bottom | juce::Justification::left, true);
        float width = project.measuresToSeconds(i + increment) * scale - x;
        if (drawHalfNotes) {
            float tickWidth = width / 2.0;
            //            g.drawText(juce::String(i + 1) + ".3.00", x + tickWidth + labelMargin, labelY, labelWidth,
            //            labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
        }
        if (drawQuarterNotes) {
            float tickWidth = width / 4.0;
            //            g.drawText(juce::String(i + 1) + ".2.00", x + tickWidth + labelMargin, labelY, labelWidth,
            //            labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
            //            g.drawText(juce::String(i + 1) + ".4.00", x + 3 * tickWidth + labelMargin, labelY, labelWidth,
            //            labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
        }
    }
}

void TimeMeter::drawTicksInSeconds(juce::Graphics &g, const juce::Rectangle<int> &bounds) const {
    auto labelY = bounds.getY() - 12;
    auto labelWidth = 75;
    auto labelHeight = 20;
    auto labelMargin = 5;
    float tickHeight = 15;
    float secondTickHeight = 7;
    float thirdTickHeight = 4;
    float scale = project.getHorizontalScale();
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    float y = bounds.getY() + (bounds.getHeight() - tickHeight);
    double secs = 0;
    int increment = std::max(1, highestPowerOf2((int)(128.0 / scale)));
    int secondTickIncrement = (increment == 1) ? 4 : std::max(2, std::min(16, increment));
    int thirdTickIncrement = std::max(4, std::min(16, increment * 4));
    bool drawHalfNotes = (float)(128.0 / scale) < 1.25;
    bool drawQuarterNotes = (float)(128.0 / scale) < 0.55;
    if (drawQuarterNotes || drawHalfNotes) {
        secondTickIncrement = thirdTickIncrement = 0;
    }

    for (int i = 0; secs * scale < bounds.getWidth(); i += increment, secs = project.measuresToSeconds(i)) {
        float x = secs * scale;
        int minutes = (int)(secs / 60);
        secs = std::fmod(secs, 60);

        //        g.drawRect(x, y, 1.0, tickHeight, 1.0);
        //        g.drawText(juce::String(minutes) + ":" + juce::String(secs, 3).paddedLeft('0', 6), x + labelMargin,
        //        labelY,
        //            labelWidth, labelHeight, juce::Justification::bottom | juce::Justification::left, true);

        float width = project.measuresToSeconds(i + increment) * scale - x;
        if (secondTickIncrement > 0) {
            float tickWidth = width / (float)secondTickIncrement;
            for (int j = 1; j < secondTickIncrement; j++) {
                //                g.drawRect(x + j * tickWidth, bounds.getY() + bounds.getHeight() -
                //                secondTickHeight, 1.0,
                //                    secondTickHeight, 1.0);
            }
        }
        if (thirdTickIncrement > 0) {
            float tickWidth = width / (float)thirdTickIncrement;
            for (int j = 1; j < thirdTickIncrement; j++) {
                //                g.drawRect(
                //                    x + j * tickWidth, bounds.getY() + bounds.getHeight() - thirdTickHeight, 1.0,
                //                    thirdTickHeight, 1.0);
            }
        }
        if (drawHalfNotes) {
            float tickWidth = width / 2.0;
            secs = project.measuresToSeconds(i + 0.5);
            int minutes = (int)(secs / 60);
            secs = std::fmod(secs, 60);
            //            g.drawRect(x + tickWidth, y, 1.0, tickHeight, 1.0);
            //            g.drawText(juce::String(minutes) + ":" + juce::String(secs, 3).paddedLeft('0', 6),
            //                x + tickWidth + labelMargin, labelY, labelWidth, labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
        }
        if (drawQuarterNotes) {
            float tickWidth = width / 4.0;
            secs = project.measuresToSeconds(i + 0.25);
            int minutes = (int)(secs / 60);
            secs = std::fmod(secs, 60);
            //            g.drawRect(x + tickWidth, y, 1.0, tickHeight, 1.0);
            //            g.drawText(juce::String(minutes) + ":" + juce::String(secs, 3).paddedLeft('0', 6),
            //                x + tickWidth + labelMargin, labelY, labelWidth, labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
            secs = project.measuresToSeconds(i + 0.75);
            minutes = (int)(secs / 60);
            secs = std::fmod(secs, 60);
            //            g.drawRect(x + 3 * tickWidth, y, 1.0, tickHeight, 1.0);
            //            g.drawText(juce::String(minutes) + ":" + juce::String(secs, 3).paddedLeft('0', 6),
            //                x +3 * tickWidth + labelMargin, labelY, labelWidth, labelHeight,
            //                juce::Justification::bottom | juce::Justification::left, true);
        }
    }
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
