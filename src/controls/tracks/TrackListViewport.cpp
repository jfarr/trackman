#include "TrackListViewport.h"

void TrackListViewport::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    DBG("TrackListViewport::visibleAreaChanged");
    auto hbounds = getHorizontalScrollBar().getBounds().withTrimmedLeft(20);
    auto vbounds = getVerticalScrollBar().getBounds().withTrimmedTop(20);
    DBG("hbounds: " << hbounds.getX() << "," << hbounds.getY() << "," << hbounds.getWidth() << "," << hbounds.getRight());
    DBG("vbounds: " << vbounds.getX() << "," << vbounds.getY() << "," << vbounds.getWidth() << "," << vbounds.getRight());
    getHorizontalScrollBar().setBounds(getHorizontalScrollBar().getBounds().withTrimmedLeft(20));
    getVerticalScrollBar().setBounds(getVerticalScrollBar().getBounds().withTrimmedTop(20));
    repaint();
}

void TrackListViewport::resized() {
    juce::Viewport::resized();
    //    getHorizontalScrollBar().setBounds(getHorizontalScrollBar().getBounds().withTrimmedLeft(20));
    //    getVerticalScrollBar().setBounds(getVerticalScrollBar().getBounds().withTrimmedTop(20));
}
