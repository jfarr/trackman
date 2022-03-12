#pragma once

#include <JuceHeader.h>

//class TrackListScrollBarLookAndFeel : public juce::LookAndFeel_V4 {
//    virtual void drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar, int x, int y, int width, int height,
//        bool isScrollbarVertical, int thumbStartPosition, int thumbSize, bool isMouseOver, bool isMouseDown) override {
//        if (isScrollbarVertical) {
//            y += 20;
//            thumbStartPosition += 20;
//            height -= 20;
//            thumbSize -= 20;
//        }
//        juce::LookAndFeel_V4::drawScrollbar(g, scrollbar, x, y, width, height, isScrollbarVertical, thumbStartPosition, thumbSize,
//            isMouseOver, isMouseDown);
//    }
//
//    virtual void drawScrollbarButton(juce::Graphics &g, juce::ScrollBar &scrollbar, int width, int height,
//        int buttonDirection, bool isScrollbarVertical, bool isMouseOverButton, bool isButtonDown) override {
//        juce::LookAndFeel_V4::drawScrollbarButton(
//            g, scrollbar, width, height, buttonDirection, isScrollbarVertical, isMouseOverButton, isButtonDown);
//    }
//};

//class TrackListScrollBar : public juce::ScrollBar {
//  public:
//    TrackListScrollBar(bool shouldBeVertical) : ScrollBar(shouldBeVertical) { setLookAndFeel(&lf); }
//    ~TrackListScrollBar() { setLookAndFeel(nullptr); }
//
//    void resized() override;
//
//  private:
//    TrackListScrollBarLookAndFeel lf;
//    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListScrollBar)
//};

class TrackListViewport : public juce::Viewport {
  public:
    TrackListViewport();
    ~TrackListViewport() {}

//    juce::ScrollBar *createScrollBarComponent(bool isVertical) override;
    void visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) override;
    void resized() override;

  private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListViewport)
};
