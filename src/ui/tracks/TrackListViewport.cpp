#include "TrackListViewport.h"
#include "ui/desktop/DesktopComponent.h"

void TrackListViewport::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    desktop.visibleAreaChanged(newVisibleArea);
}
