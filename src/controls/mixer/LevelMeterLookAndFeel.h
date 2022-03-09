#pragma once

#include <ff_meters.h>

#include "model/Track.h"

class LevelMeterLookAndFeel : public foleys::LevelMeterLookAndFeel {
  public:
    LevelMeterLookAndFeel(Track &track) : track(track) {}
    ~LevelMeterLookAndFeel() {}

    juce::Rectangle<float> drawBackground(
        juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {
        g.setColour(juce::Colour{0xff282828});
        if (meterType & foleys::LevelMeter::HasBorder) {
            const auto corner = std::min(bounds.getWidth(), bounds.getHeight()) * 0.01f;
            g.fillRoundedRectangle(bounds, corner);
            g.setColour(findColour(foleys::LevelMeter::lmOutlineColour));
            g.drawRoundedRectangle(bounds.reduced(3), corner, 2);
            return bounds.reduced(3 + corner);
        } else {
            g.fillRect(bounds);
            return bounds;
        }
    }

    /** Override this callback to define the placement of the actual meter bar. */
    juce::Rectangle<float> getMeterBarBounds(
        juce::Rectangle<float> bounds, foleys::LevelMeter::MeterFlags meterType) const override {
        const auto top = bounds.getY() + bounds.getWidth() * 0.5f;
        const auto bottom =
            (meterType & foleys::LevelMeter::MaxNumber) ? bounds.getBottom() - bounds.getWidth() : bounds.getBottom();
        return juce::Rectangle<float>(bounds.getX(), top, bounds.getWidth(), bottom - top);
    }

    void drawMeterBars(juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds,
        const foleys::LevelMeterSource *source, int fixedNumChannels = -1, int selectedChannel = -1) override {
        if (source == nullptr || track.isSilenced())
            return;

        const juce::Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
        const int numChannels = source->getNumChannels();
        if (meterType & foleys::LevelMeter::Minimal) {
            const float width = innerBounds.getWidth() / numChannels;
            juce::Rectangle<float> meter = innerBounds.withWidth(width);
            for (int channel = 0; channel < numChannels; ++channel) {
                meter.setX(width * channel);
                {
                    juce::Rectangle<float> meterBarBounds = getMeterBarBounds(meter, meterType);
                    drawMeterBar(g, meterType, getMeterBarBounds(meter, meterType), source->getRMSLevel(channel),
                        source->getMaxLevel(channel));
                    const float reduction = source->getReductionLevel(channel);
                    if (reduction < 1.0)
                        drawMeterReduction(
                            g, meterType, meterBarBounds.withLeft(meterBarBounds.getCentreX()), reduction);
                }
                juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
                if (!clip.isEmpty())
                    drawClipIndicator(g, meterType, clip, source->getClipFlag(channel));
                juce::Rectangle<float> maxNum = getMeterMaxNumberBounds(
                    innerBounds.withWidth(innerBounds.getWidth() / numChannels)
                        .withX(innerBounds.getX() + channel * (innerBounds.getWidth() / numChannels)),
                    meterType);
                if (!maxNum.isEmpty())
                    drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel(channel));
            }
            juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter.withX(0).withWidth(meter.getWidth() * numChannels), meterType);
            if (!ticks.isEmpty())
                drawTickMarks(g, meterType, ticks);
        }
    }

    void drawMeterBarsBackground(juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType,
        juce::Rectangle<float> bounds, int numChannels, int fixedNumChannels) override {
        const juce::Rectangle<float> innerBounds = getMeterInnerBounds(bounds, meterType);
        const float width = innerBounds.getWidth() / numChannels;
        juce::Rectangle<float> meter = innerBounds.withWidth(width);
        for (int channel = 0; channel < numChannels; ++channel) {
            meter.setX(width * channel);
            drawMeterBarBackground(g, meterType, getMeterBarBounds(meter, meterType));
            juce::Rectangle<float> clip = getMeterClipIndicatorBounds(meter, meterType);
            if (!clip.isEmpty())
                drawClipIndicatorBackground(g, meterType, clip);
        }
        juce::Rectangle<float> ticks = getMeterTickmarksBounds(meter.withX(0).withWidth(meter.getWidth() * numChannels), meterType);
        if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
    }

    void drawTickMarks(
        juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {
        const auto infinity = meterType & foleys::LevelMeter::Reduction ? -30.0f : -100.0f;

        g.setColour(juce::Colour{0xff282828});
        if (meterType & foleys::LevelMeter::Minimal) {
            const auto h = (bounds.getHeight() - 2.0f) * 0.1f;
            for (int i = 0; i < 11; ++i) {
                g.drawHorizontalLine(juce::roundToInt(bounds.getY() + i * h + 1), bounds.getX() + 2, bounds.getRight() - 2);
            }
            if (h > 9 && bounds.getWidth() > 20) {
                // don't print tiny numbers
                g.setFont(h * 0.5f);
                for (int i = 0; i < 10; ++i) {
                    g.drawFittedText(juce::String(i * 0.1 * infinity), juce::roundToInt(bounds.getX()),
                        juce::roundToInt(bounds.getY() + i * h + 2), juce::roundToInt(bounds.getWidth()),
                        juce::roundToInt(h * 0.6f), juce::Justification::centredTop, 1);
                }
            }
        }
    }

    void drawMeterChannelBackground(
        juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {
        juce::Rectangle<float> meter = getMeterBarBounds(bounds, meterType);
        if (!meter.isEmpty())
            drawMeterBarBackground(g, meterType, meter);

        juce::Rectangle<float> clip = getMeterClipIndicatorBounds(bounds, meterType);
        if (!clip.isEmpty())
            drawClipIndicatorBackground(g, meterType, clip);

        juce::Rectangle<float> ticks = getMeterTickmarksBounds(bounds, meterType);
        if (!ticks.isEmpty())
            drawTickMarks(g, meterType, ticks);
    }

    void drawMeterBarBackground(
        juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {
        juce::ignoreUnused(meterType);
        g.setColour(findColour(foleys::LevelMeter::lmMeterBackgroundColour));
        g.fillRect(bounds);
    }

    juce::Rectangle<float> getMeterClipIndicatorBounds(
        juce::Rectangle<float> bounds, foleys::LevelMeter::MeterFlags meterType) const override {
        const auto w = bounds.getWidth();
        return juce::Rectangle<float>(bounds.getX(), bounds.getY(), w, w * 0.5f);
    }

    void drawClipIndicatorBackground(
        juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds) override {
        juce::ignoreUnused(meterType);
    }

    void drawClipIndicator(juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds,
        bool hasClipped) override {
        juce::ignoreUnused(meterType);

        if (hasClipped) {
            g.setColour(findColour(foleys::LevelMeter::lmBackgroundClipColour));
            g.fillRect(bounds);
        }
    }

    void drawMeterBar(juce::Graphics &g, foleys::LevelMeter::MeterFlags meterType, juce::Rectangle<float> bounds,
        float rms, float peak) override {
        const auto infinity = meterType & foleys::LevelMeter::Reduction ? -30.0f : -100.0f;
        const auto rmsDb = juce::Decibels::gainToDecibels(rms, infinity);
        const auto peakDb = juce::Decibels::gainToDecibels(peak, infinity);

        const juce::Rectangle<float> floored(ceilf(bounds.getX()) + 1.0f, ceilf(bounds.getY()) + 1.0f,
            floorf(bounds.getRight()) - (ceilf(bounds.getX() + 2.0f)),
            floorf(bounds.getBottom()) - (ceilf(bounds.getY()) + 2.0f));

        float alpha = 0.7;
        if (verticalGradient.getNumColours() < 2) {
            verticalGradient = juce::ColourGradient(
                findColour(foleys::LevelMeter::lmMeterGradientLowColour).withAlpha(alpha), floored.getX(),
                floored.getBottom(), findColour(foleys::LevelMeter::lmMeterGradientMaxColour).withAlpha(alpha),
                floored.getX(), floored.getY(), false);
            verticalGradient.addColour(0.5f, findColour(foleys::LevelMeter::lmMeterGradientLowColour).withAlpha(alpha));
            verticalGradient.addColour(
                0.75f, findColour(foleys::LevelMeter::lmMeterGradientMidColour).withAlpha(alpha));
        }
        g.setGradientFill(verticalGradient);
        g.fillRect(floored.withTop(floored.getY() + rmsDb * floored.getHeight() / infinity));

        if (peakDb > -49.0f) {
            g.setColour((peakDb > -0.3f) ? juce::Colours::red
                                         : ((peakDb > -5.0f) ? juce::Colours::orange : juce::Colours::lightgreen));
            g.drawHorizontalLine(
                juce::roundToInt(floored.getY() + juce::jmax(peakDb * floored.getHeight() / infinity, 0.0f)),
                floored.getX(), floored.getRight());
        }
    }

    void updateMeterGradients() override {
        horizontalGradient.clearColours();
        verticalGradient.clearColours();
    }

  private:
    Track &track;
    juce::ColourGradient horizontalGradient;
    juce::ColourGradient verticalGradient;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LevelMeterLookAndFeel)
};
