#pragma once

#include <JuceHeader.h>

#include "SampleListener.h"
#include "TrackLaneControl.h"
#include "model/Track.h"
#include "model/TrackList.h"
#include "ui/common/PositionOverlay.h"
#include "ui/desktop/TrackListListener.h"

using namespace std;
using namespace juce;

namespace trackman {

class DesktopController;

class DropBox : public Component {
  public:
    DropBox(function<bool()> canDropFn) : canDropFn(canDropFn) { setSize(200, 60); }
    ~DropBox() override {}

    void setSource(unique_ptr<AudioFormatReaderSource> &newSource) { source = move(newSource); }

    void paint(Graphics &g) override {
        g.fillAll(canDropFn() ? Colours::dimgrey : Colours::darkred);
        g.setColour(Colours::grey);
        g.drawRect(0, 0, getWidth(), getHeight());
    }

  private:
    unique_ptr<AudioFormatReaderSource> source;
    function<bool()> canDropFn;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DropBox)
};

class TrackListPanel : public Component,
                       public DragAndDropContainer,
                       public DragAndDropTarget,
                       public Timer {
  public:
    TrackListPanel(
        DesktopController &desktopController, Viewport &viewport, AudioTransportSource &transport);
    ~TrackListPanel() override;

    Track *getTrackAtPos(int x, int y);

    void addLane(TrackLaneControl *lane) { lanes.push_back(lane); }
    void removeLane(TrackLaneControl *lane) { lanes.remove(lane); }
    void clear() { lanes.clear(); }
    void resize();
    void increaseMaxWidth(int increment) {
        maxWidth += increment;
        resize();
    }
    void update();
    void resizePositionOverlay();

    void fileDragEnter(const StringArray &files, int x, int y);
    void fileDragMove(const StringArray &files, int x, int y);
    void fileDragExit(const StringArray &files);
    void filesDropped(const StringArray &files, int x, int y);

    void addListener(SampleListener *listener);
    void removeListener(SampleListener *listener);
    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // DragAndDropTarget
    bool isInterestedInDragSource(const SourceDetails &dragSourceDetails) override { return true; }
    void itemDropped(const SourceDetails &dragSourceDetails) override;

    //==============================================================================
    // DragAndDropContainer
    void dragOperationStarted(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;
    void dragOperationEnded(const DragAndDropTarget::SourceDetails &dragSourceDetails) override;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;
    void mouseDown(const MouseEvent &event) override;

  private:
    DesktopController &desktopController;
    Viewport &viewport;
    AudioTransportSource &transport;
    list<TrackLaneControl *> lanes;
    PositionOverlay overlay;
    int maxWidth = 0;

    DropBox dropBox;
    int dragSourceOffset = 0;
    Point<int> dragPosition;
    bool dragging = false;
    bool canDrop();

    list<SampleListener *> sampleListeners;
    list<TrackListListener *> trackListListeners;

    int getPanelWidth() const;
    int getPanelHeight() const;
    int getTrackLaneHeight() const;

    void notifySampleDropped(SampleThumbnail *thumbnail, int x, int y);
    void notifySampleResized(SampleThumbnail *thumbnail, int width);
    void notifyDragEnded();
    void notifySelectionChanged();

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListPanel)
};

} // namespace trackman
