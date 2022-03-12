#pragma once

#include <JuceHeader.h>

class DecibelSliderLookAndFeel : public juce::LookAndFeel_V4 {
  public:
    juce::Label *createSliderTextBox(juce::Slider &slider) override {
        auto l = LookAndFeel_V4::createSliderTextBox(slider);
        l->setFont(l->getFont().withHeight(8));
        return l;
    }

    void drawLinearSlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float minSliderPos,
        float maxSliderPos, const juce::Slider::SliderStyle style, juce::Slider &slider) override {

        juce::LookAndFeel_V4::drawLinearSlider(
            g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);

        auto buttonRect = juce::Rectangle<int>(13, height / 6).withCentre(juce::Point<int>((width - x) / 2, sliderPos));
        auto r = buttonRect;

        // draw button gradients
        auto topRect = r.removeFromTop(3);
        auto bottomRect = r.removeFromBottom(6);
        juce::ColourGradient gradient =
            juce::ColourGradient::vertical(juce::Colours::silver, topRect.getY(), juce::Colours::darkgrey, topRect.getBottom());
        g.setGradientFill(gradient);
        g.fillRect(topRect);
        gradient = juce::ColourGradient::vertical(
            juce::Colours::dimgrey, bottomRect.getY(), juce::Colour{ 0xff101010 }, bottomRect.getBottom());
        g.setGradientFill(gradient);
        g.fillRect(bottomRect);
        gradient = juce::ColourGradient::vertical(
            juce::Colours::darkgrey, r.getY(), juce::Colours::silver, r.getBottom() + bottomRect.getHeight());
        g.setGradientFill(gradient);
        g.fillRect(r);

        // draw button lines
        g.setColour(juce::Colours::silver);
        g.drawHorizontalLine(topRect.getBottom(), topRect.getX(), topRect.getRight());
        g.drawHorizontalLine(bottomRect.getY(), bottomRect.getX(), bottomRect.getRight());
        auto lineY = r.getHeight() / 2 + r.getY() + 1;
        g.setColour(juce::Colours::white);
        g.drawLine(r.getX(), lineY, r.getRight(), lineY);
        if (r.getHeight() > 10) {
            g.setColour(juce::Colours::silver);
            auto lineY2 = topRect.getBottom() + (lineY - topRect.getBottom()) / 3 + 1;
            g.drawLine(r.getX() + 1, lineY2, r.getRight() - 1, lineY2, 0.5);
            lineY2 = topRect.getBottom() + (lineY - topRect.getBottom()) * 2 / 3;
            g.drawLine(r.getX() + 1, lineY2, r.getRight() - 1, lineY2, 0.5);
            g.setColour(juce::Colours::darkgrey);
            lineY2 = lineY + (lineY - topRect.getBottom()) / 3 + 1;
            g.drawLine(r.getX() + 1, lineY2, r.getRight() - 1, lineY2, 0.5);
            lineY2 = lineY + (lineY - topRect.getBottom()) * 2 / 3;
            g.drawLine(r.getX() + 1, lineY2, r.getRight() - 1, lineY2, 0.5);
        }

        // draw ticks
        g.setColour(juce::Colours::grey);
        g.setFont(g.getCurrentFont().withHeight(10));
        auto tickX = width - 15;
        auto tickWidth = 15;
        auto tickHeight = 8;
        auto tickOffset = tickHeight / 2;
        g.drawFittedText("12", tickX, y + height * (1 - slider.valueToProportionOfLength(12)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
        g.drawFittedText("6", tickX, y + height * (1 - slider.valueToProportionOfLength(6)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
        g.drawFittedText("0", tickX, y + height * (1 - slider.valueToProportionOfLength(0)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
        g.drawFittedText("-9", tickX, y + height * (1 - slider.valueToProportionOfLength(-9)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
        g.drawFittedText("-24", tickX, y + height * (1 - slider.valueToProportionOfLength(-24)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
        g.drawFittedText("-48", tickX, y + height * (1 - slider.valueToProportionOfLength(-48)) - tickOffset, tickWidth,
            tickHeight, juce::Justification::topRight, 1, 0.0);
    }
};

class SliderListener {
  public:
    virtual void sliderClicked() = 0;
};

class DecibelSlider : public juce::Slider {
  public:
    DecibelSlider() {
        setLookAndFeel(&lf);
        setSliderStyle(juce::Slider::LinearVertical);
        setRange(-100, 12);
        setTextBoxStyle(juce::Slider::TextBoxAbove, false, 35, 13);
        setSkewFactorFromMidPoint(-10.0);
    }
    ~DecibelSlider() { setLookAndFeel(nullptr); }

    void setListener(SliderListener *newListener) { listener = newListener; }

    void mouseDown(const juce::MouseEvent &event) override {
        if (listener != nullptr) {
            listener->sliderClicked();
        }
        Slider::mouseDown(event);
    }

    double getValueFromText(const juce::String &text) override {
        auto minusInfinitydB = -100.0;

        auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();

        return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB : decibelText.getDoubleValue();
    }

    juce::String getTextFromValue(double value) override { return juce::Decibels::toString(value); }

  private:
    SliderListener *listener;
    DecibelSliderLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelSlider)
};
