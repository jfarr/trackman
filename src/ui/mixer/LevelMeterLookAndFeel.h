#pragma once

#include <ff_meters.h>

#include "model/Track.h"

using namespace std;
using namespace juce;
using namespace foleys;

namespace trackman {

class LevelMeterLookAndFeel : public foleys::LevelMeterLookAndFeel {
  public:
    LevelMeterLookAndFeel() {}
    ~LevelMeterLookAndFeel() {}

    Rectangle<float> drawBackground(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds) override {
        g.setColour(Colour{0xff282828});
        if (meterType & LevelMeter::HasBorder) {
            const auto corner = min(bounds.getWidth(), bounds.getHeight()) * 0.01f;
            g.fillRoundedRectangle(bounds, corner);
            g.setColour(findColour(LevelMeter::lmOutlineColour));
            g.drawRoundedRectangle(bounds.reduced(3), corner, 2);
            return bounds.reduced(3 + corner);
        } else {
            g.fillRect(bounds);
            return bounds;
        }
    }

    /** Override this callback to define the placement of the actual meter bar. */
    Rectangle<float> getMeterBarBounds(Rectangle<float> bounds, LevelMeter::MeterFlags meterType) const override {
        const auto top = bounds.getY() + bounds.getWidth() * 0.5f;
        const auto bottom =
            (meterType & LevelMeter::MaxNumber) ? bounds.getBottom() - bounds.getWidth() : bounds.getBottom();
        return Rectangle<float>(bounds.getX(), top, bounds.getWidth(), bottom - top);
    }

    void drawMeterBars(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds,
        const LevelMeterSource *source, int fixedNumChannels = -1, int selectedChannel = -1) override {
        if (source == nullptr)
            return;

        const Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
        const int numChannels = source->getNumChannels();
        if (meterType & LevelMeter::Minimal) {
            const float width = innerBounds.getWidth() / numChannels;
            Rectangle<float> meter = innerBounds.withWidth(width);
            for (int channel = 0; channel < numChannels; ++channel) {
                meter.setX(width * channel);
                {
                    Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
                    drawMeterBar(g, meterType, getMeterBarBounds(meter, meterType), source->getRMSLevel(channel),
                        source->getMaxLevel(channel));
                    const float reduction = source->getReductionLevel(channel);
                    if (reduction < 1.0)
                        drawMeterReduction(
                            g, meterType, meterBarBounds.withLeft(meterBarBounds.getCentreX()), reduction);
                }
                Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
                if (!clip.isEmpty())
                    drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
                Rectangle<float> maxNum = getMeterMaxNumberBounds(
                    innerBounds.withWidth(innerBounds.getWidth() / numChannels)
                        .withX(innerBounds.getX() + channel * (innerBounds.getWidth() / numChannels)),
                    meterType);
                if (!maxNum.isEmpty())
                    drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));
            }
            Rectangle<float> ticks =
                getMeterTickmarksBounds(meter.withX(0).withWidth(meter.getWidth() * numChannels), meterType);
            if (!ticks.isEmpty())
                drawTickMarks(g, meterType, ticks);
        }
    }

    void drawMeterBarsBackground(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds,
        int numChannels, int fixedNumChannels) override {
        const Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
        const float width = innerBounds.getWidth() / numChannels;
        Rectangle<float> meter = innerBounds.withWidth(width);
        for (int channel = 0; channel < numChannels; ++channel) {
            meter.setX(width * channel);
            drawMeterBarBackground(g, meterType, getMeterBarBounds(meter, meterType));
            Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
            if (!clip.isEmpty())
                drawClipIndicatorBackground(g, meterType, clip);
        }
        Rectangle<float> ticks =
            getMeterTickmarksBounds(meter.withX(0).withWidth(meter.getWidth() * numChannels), meterType);
        if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
    }

    void drawTickMarks(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds) override {
        const auto infinity = meterType & LevelMeter::Reduction ? -30.0f : -100.0f;

        g.setColour(Colour{0xff282828});
        if (meterType & LevelMeter::Minimal) {
            const auto h = (bounds.getHeight() - 2.0f) * 0.1f;
            for (int i = 0; i < 11; ++i) {
                g.drawHorizontalLine(roundToInt(bounds.getY() + i * h + 1), bounds.getX() + 2, bounds.getRight() - 2);
            }
            if (h > 9 && bounds.getWidth() > 20) {
                // don't print tiny numbers
                g.setFont(h * 0.5f);
                for (int i = 0; i < 10; ++i) {
                    g.drawFittedText(String(i * 0.1 * infinity), roundToInt(bounds.getX()),
                        roundToInt(bounds.getY() + i * h + 2), roundToInt(bounds.getWidth()), roundToInt(h * 0.6f),
                        Justification::centredTop, 1);
                }
            }
        }
    }

    void drawMeterChannelBackground(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds) override {
        Rectangle<float> meter = getMeterBarBounds(bounds, meterType);
        if (!meter.isEmpty())
            drawMeterBarBackground(g, meterType, meter);

        Rectangle<float> clip = getMeterClipIndicatorBounds(bounds, meterType);
        if (!clip.isEmpty())
            drawClipIndicatorBackground(g, meterType, clip);

        Rectangle<float> ticks = getMeterTickmarksBounds(bounds, meterType);
        if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
    }

    void drawMeterBarBackground(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds) override {
        ignoreUnused(meterType);
        g.setColour(findColour(LevelMeter::lmMeterBackgroundColour));
        g.fillRect(bounds);
    }

    Rectangle<float> getMeterClipIndicatorBounds(
        Rectangle<float> bounds, LevelMeter::MeterFlags meterType) const override {
        const auto w = bounds.getWidth();
        return Rectangle<float>(bounds.getX(), bounds.getY(), w, w * 0.5f);
    }

    void drawClipIndicatorBackground(Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds) override {
        ignoreUnused(meterType);
    }

    void drawClipIndicator(
        Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds, bool hasClipped) override {
        ignoreUnused(meterType);

        if (hasClipped) {
            g.setColour(findColour(LevelMeter::lmBackgroundClipColour));
            g.fillRect(bounds);
        }
    }

    void drawMeterBar(
        Graphics &g, LevelMeter::MeterFlags meterType, Rectangle<float> bounds, float rms, float peak) override {
        const auto infinity = meterType & LevelMeter::Reduction ? -30.0f : -100.0f;
        const auto rmsDb = Decibels::gainToDecibels(rms, infinity);
        const auto peakDb = Decibels::gainToDecibels(peak, infinity);

        const Rectangle<float> floored(ceilf(bounds.getX()) + 1.0f, ceilf(bounds.getY()) + 1.0f,
            floorf(bounds.getRight()) - (ceilf(bounds.getX() + 2.0f)),
            floorf(bounds.getBottom()) - (ceilf(bounds.getY()) + 2.0f));

        float alpha = 0.7;
        if (verticalGradient.getNumColours() < 2) {
            verticalGradient = ColourGradient(findColour(LevelMeter::lmMeterGradientLowColour).withAlpha(alpha),
                floored.getX(), floored.getBottom(), findColour(LevelMeter::lmMeterGradientMaxColour).withAlpha(alpha),
                floored.getX(), floored.getY(), false);
            verticalGradient.addColour(0.5f, findColour(LevelMeter::lmMeterGradientLowColour).withAlpha(alpha));
            verticalGradient.addColour(0.75f, findColour(LevelMeter::lmMeterGradientMidColour).withAlpha(alpha));
        }
        g.setGradientFill(verticalGradient);
        g.fillRect(floored.withTop(floored.getY() + rmsDb * floored.getHeight() / infinity));

        if (peakDb > -49.0f) {
            g.setColour((peakDb > -0.3f) ? Colours::red : ((peakDb > -5.0f) ? Colours::orange : Colours::lightgreen));
            g.drawHorizontalLine(roundToInt(floored.getY() + jmax(peakDb * floored.getHeight() / infinity, 0.0f)),
                floored.getX(), floored.getRight());
        }
    }

    void updateMeterGradients() override {
        horizontalGradient.clearColours();
        verticalGradient.clearColours();
    }

  private:
    ColourGradient horizontalGradient;
    ColourGradient verticalGradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};

} // namespace trackman
