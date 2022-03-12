#include "TrackListViewport.h"

//void TrackListScrollBar::resized() {
//    juce::ScrollBar::resized();
//}
//
TrackListViewport::TrackListViewport() {
//    recreateScrollbars();
};
//
//juce::ScrollBar *TrackListViewport::createScrollBarComponent(bool isVertical) {
//    return new TrackListScrollBar(isVertical);
//}

void TrackListViewport::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
        DBG("TrackListViewport::visibleAreaChanged");
        juce::MessageManager::callAsync([this]() {
            DBG("update scroll bars");
            auto hbounds = getHorizontalScrollBar().getParentComponent()->getBounds().withTrimmedLeft(20);
            auto vbounds = getVerticalScrollBar().getParentComponent()->getBounds().withTrimmedTop(20);
            DBG("hbounds: " << hbounds.getX() << "," << hbounds.getY() << "," << hbounds.getWidth() << "," <<
                hbounds.getRight()); DBG("vbounds: " << vbounds.getX() << "," << vbounds.getY() << "," << vbounds.getWidth()
                            << "," << vbounds.getRight());
            getHorizontalScrollBar().setBounds(getHorizontalScrollBar().getBounds().withLeft(hbounds.getX()));
            getVerticalScrollBar().setBounds(getVerticalScrollBar().getBounds().withTop(hbounds.getY()));
        });
    //    repaint();
}

void TrackListViewport::resized() {
    DBG("TrackListViewport::resized");
    juce::Viewport::resized();
    //    getHorizontalScrollBar().setBounds(getHorizontalScrollBar().getBounds().withTrimmedLeft(20));
    //    getVerticalScrollBar().setBounds(getVerticalScrollBar().getBounds().withTrimmedTop(20));
}
