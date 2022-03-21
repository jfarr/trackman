#include "TrackListViewport.h"
#include "ui/desktop/DesktopComponent.h"

void TrackListViewport::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    desktop.visibleAreaChanged(newVisibleArea);
}

void TrackListViewport::timerCallback() {
    auto pos = project.getTransport().getTransportSource().getCurrentPosition() * project.getHorizontalScale();
    auto x = getViewPositionX();
    auto w = getViewWidth();
    if (pos > x + w) {
        auto trackListPanel = (TrackListPanel *) getViewedComponent();
        trackListPanel->increaseMaxWidth(w);
        setViewPosition(juce::Point<int>(x + w, getViewPositionY()));
    }
}
