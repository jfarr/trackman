#include "TimeMeter.h"
#include "common/mathutil.h"
#include "common/timeutil.h"

TimeMeter::TimeMeter(Project &project) : project(project) { setInterceptsMouseClicks(false, false); }

void TimeMeter::paint(juce::Graphics &g) {
    auto bounds = getLocalBounds();
    const int topStripHeight = 30;
    const int measureLabelY = bounds.getY() - 7;
    const int timeLabelY = bounds.getY() + 3;
    const int labelWidth = 75;
    const int labelHeight = 20;
    const int labelMargin = 5;

    g.setColour(juce::Colours::grey);
    g.fillRect(0, 0, bounds.getWidth(), topStripHeight);

    const float scale = project.getHorizontalScale();
    const float numerator = project.getTimeSignature().getNumerator();
    const float denominator = project.getTimeSignature().getDenominator();
    const float dashes[]{0.5, 0.5};
    const int increment = std::max(1, (int)(highestPowerOf2((int)(120.0 / scale * denominator / numerator))));
    const bool drawNotes = increment <= 4;
    const float width = project.measuresToSeconds(increment + 1) * scale;

    for (int measure = 1, x = project.measuresToSeconds(measure) * scale; x < bounds.getWidth();
         measure += increment, x = project.measuresToSeconds(measure) * scale) {
        auto line = juce::Line<float>(x, 0.0, x, bounds.getHeight());
        g.setColour(juce::Colours::grey);
        g.drawDashedLine(line, dashes, 2, 1.0);
        if (drawNotes) {
            int numNotes = increment * numerator;
            float noteWidth = width / numNotes;
            for (int j = 1; j < numNotes; j++) {
                float noteX = x + j * noteWidth;
                line = juce::Line<float>(noteX, 0.0, noteX, bounds.getHeight());
                g.drawDashedLine(line, dashes, 2, 0.5);
            }
        } else {
            int numMeasures = 8;
            float measureWidth = width / numMeasures;
            for (int j = 1; j < numMeasures; j++) {
                float noteX = x + j * measureWidth;
                line = juce::Line<float>(noteX, 0.0, noteX, bounds.getHeight());
                g.drawDashedLine(line, dashes, 2, 0.5);
            }
        }
        g.setColour(juce::Colour{0xff282828});
        line = juce::Line<float>(x, 0.0, x, topStripHeight);
        g.drawLine(line);

    }
    g.setColour(juce::Colour{0xff282828});
    g.setFont(11);
    for (int measure = 1, x = project.measuresToSeconds(measure) * scale; x < bounds.getWidth();
         measure += increment, x = project.measuresToSeconds(measure) * scale) {
        double secs = project.measuresToSeconds(measure);
        g.drawText(juce::String(measure), x + labelMargin, measureLabelY, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::left, true);
        auto t = juce::RelativeTime(secs);
        g.drawText(::formatSecsAsTime(secs), x + labelMargin, timeLabelY, labelWidth, labelHeight,
            juce::Justification::bottom | juce::Justification::left, true);
    }
    drawStartMarker(g);
}

void TimeMeter::drawStartMarker(juce::Graphics &g) const {
    auto margin = 4;
    auto arrowHeight = 8.0;
    auto arrowWidth = 18.0;

    float audioPosition = 0.0;
    float drawPosition = audioPosition * project.getHorizontalScale();

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
