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
