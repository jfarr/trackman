#pragma once

#include "JuceHeader.h"

class SliderListener {
  public:
    virtual void onSliderClick() = 0;
};

class DecibelSlider : public juce::Slider {
  public:
    DecibelSlider() {
        setSliderStyle(juce::Slider::LinearVertical);
        setRange(-100, 12);
        setTextBoxStyle(juce::Slider::TextBoxAbove, false, 65, 14);
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelSlider)
};
