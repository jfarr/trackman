#include "TrackListViewport.h"
#include "controls/desktop/DesktopComponent.h"

void TrackListViewport::visibleAreaChanged(const juce::Rectangle<int> &newVisibleArea) {
    desktop.visibleAreaChanged(newVisibleArea);
}
