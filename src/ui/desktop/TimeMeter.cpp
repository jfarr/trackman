#include "TimeMeter.h"
#include "common/mathutil.h"
#include "common/timeutil.h"

namespace trackman {

TimeMeter::TimeMeter(Project &project) : project(project) { setInterceptsMouseClicks(false, false); }

void TimeMeter::paint(Graphics &g) {
    auto bounds = getLocalBounds();
    const int topStripHeight = 30;
    const int measureLabelY = bounds.getY() - 7;
    const int timeLabelY = bounds.getY() + 3;
    const int labelWidth = 75;
    const int labelHeight = 20;
    const int labelMargin = 5;

    g.setColour(Colours::grey);
    g.fillRect(0, 0, bounds.getWidth(), topStripHeight);

    const float scale = project.getHorizontalScale();
    const float numerator = project.getTimeSignature().getNumerator();
    const float denominator = project.getTimeSignature().getDenominator();
    const float dashes[]{0.5, 0.5};
    const int increment = max(1, (int)(highestPowerOf2((int)(120.0 / scale * denominator / numerator))));
    const bool drawNotes = increment <= 4;
    const float width = project.measuresToSeconds(increment + 1) * scale;

    for (int measure = 1, x = project.measuresToSeconds(measure) * scale; x < bounds.getWidth();
         measure += increment, x = project.measuresToSeconds(measure) * scale) {
        auto line = Line<float>(x, 0.0, x, bounds.getHeight());
        g.setColour(Colours::grey);
        g.drawDashedLine(line, dashes, 2, 1.0);
        if (drawNotes) {
            int numNotes = increment * numerator;
            float noteWidth = width / numNotes;
            for (int j = 1; j < numNotes; j++) {
                float noteX = x + j * noteWidth;
                line = Line<float>(noteX, 0.0, noteX, bounds.getHeight());
                g.drawDashedLine(line, dashes, 2, 0.5);
            }
        } else {
            int numMeasures = 8;
            float measureWidth = width / numMeasures;
            for (int j = 1; j < numMeasures; j++) {
                float noteX = x + j * measureWidth;
                line = Line<float>(noteX, 0.0, noteX, bounds.getHeight());
                g.drawDashedLine(line, dashes, 2, 0.5);
            }
        }
        g.setColour(Colour{0xff282828});
        line = Line<float>(x, 0.0, x, topStripHeight);
        g.drawLine(line);
    }
    g.setColour(Colour{0xff282828});
    g.setFont(11);
    for (int measure = 1, x = project.measuresToSeconds(measure) * scale; x < bounds.getWidth();
         measure += increment, x = project.measuresToSeconds(measure) * scale) {
        double secs = project.measuresToSeconds(measure);
        g.drawText(String(measure), x + labelMargin, measureLabelY, labelWidth, labelHeight,
            Justification::bottom | Justification::left, true);
        auto t = RelativeTime(secs);
        g.drawText(timeutil::formatSecsAsTime(secs), x + labelMargin, timeLabelY, labelWidth, labelHeight,
            Justification::bottom | Justification::left, true);
    }
    drawStartMarker(g);
}

void TimeMeter::drawStartMarker(Graphics &g) const {
    auto margin = 4;
    auto arrowHeight = 8.0;
    auto arrowWidth = 18.0;

    float audioPosition = 0.0;
    float drawPosition = audioPosition * project.getHorizontalScale();

    g.setColour(Colours::lightgrey);
    g.drawLine(drawPosition, margin, drawPosition, (float)getHeight(), 1.0f);

    auto a = Point<float>(drawPosition - arrowWidth / 2, margin);
    auto b = Point<float>(drawPosition + arrowWidth / 2, margin);
    auto c = Point<float>(drawPosition, margin + arrowHeight);
    Path path;
    path.addTriangle(a, b, c);
    g.setColour(Colours::lightgrey);
    g.fillPath(path);

    arrowWidth *= 0.8;
    arrowHeight *= 0.8;
    margin += arrowHeight * 0.2;
    a = Point<float>(drawPosition - arrowWidth / 2, margin);
    b = Point<float>(drawPosition + arrowWidth / 2, margin);
    c = Point<float>(drawPosition, margin + arrowHeight);
    Path path2;
    path2.addTriangle(a, b, c);
    g.setColour(Colours::steelblue);
    g.fillPath(path2);
}

} // namespace trackman
