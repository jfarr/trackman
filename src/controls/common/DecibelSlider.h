#pragma once

#include "JuceHeader.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
    juce::Label* createSliderTextBox (juce::Slider& slider) override {
        auto l = LookAndFeel_V4::createSliderTextBox(slider);
        l->setFont(l->getFont().withHeight(8));
        return l;
    }
};

class SliderListener {
  public:
    virtual void onSliderClick() = 0;
};

class DecibelSlider : public juce::Slider {
  public:
    DecibelSlider() {
        setLookAndFeel(&lf);
        setSliderStyle(juce::Slider::LinearVertical);
        setRange(-100, 12);
        setTextBoxStyle(juce::Slider::TextBoxAbove, false, 35, 11);
        setSkewFactorFromMidPoint(-10.0);
    }

    void setListener(SliderListener *newListener) { listener = newListener; }

    void mouseDown(const juce::MouseEvent &event) override {
        if (listener != nullptr) {
            listener->onSliderClick();
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
    CustomLookAndFeel lf;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelSlider)
};
