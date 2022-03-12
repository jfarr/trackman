#pragma once

#include <JuceHeader.h>

#include "SampleListener.h"
#include "TrackLaneControl.h"
#include "controls/common/PositionOverlay.h"
#include "controls/common/TimeMeter.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Track.h"
#include "model/TrackList.h"

class DropBox : public juce::Component {
  public:
    DropBox() { setSize(200, 85); }
    ~DropBox() {}

    void setSource(std::unique_ptr<juce::AudioFormatReaderSource> &newSource) { source = std::move(newSource); }

    void paint(juce::Graphics &g) override {
        g.fillAll(juce::Colours::dimgrey);
        g.setColour(juce::Colours::grey);
        g.drawRect(0, 0, getWidth(), getHeight());
    }

  private:
    std::unique_ptr<juce::AudioFormatReaderSource> source;
};

class TrackListPanel : public juce::Component,
                       public juce::DragAndDropContainer,
                       public juce::DragAndDropTarget,
                       private juce::Timer {
  public:
    TrackListPanel(Project &project, TrackList &trackList, juce::Viewport &viewport, juce::AudioTransportSource &transport,
        juce::AudioFormatManager &formatManager);
    ~TrackListPanel();

    Track *getTrackAtPos(int x, int y);

    void addLane(TrackLaneControl *lane) { lanes.push_back(lane); }
    void resize();
    void clear() { lanes.clear(); }

    void update();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped(const juce::StringArray &files, int x, int y);

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
    void paint(juce::Graphics &g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    Project &project;
    TrackList &trackList;
    juce::Viewport &viewport;
    juce::AudioTransportSource &transport;
    juce::AudioFormatManager &formatManager;
    std::list<TrackLaneControl *> lanes;

    DropBox dropBox;
    int dragSourceOffset;

    std::list<SampleListener *> sampleListeners;
    std::list<TrackListListener *> trackListListeners;

    int getTrackLaneWidth() const;
    int getTrackLaneHeight() const;

    void notifySampleDropped(SampleThumbnail *thumbnail, int x, int y);
    void notifySampleResized(SampleThumbnail *thumbnail, int width);
    void notifyDragEnded();
    void notifySelectionChanged();

    //==============================================================================
    // Timer
    void timerCallback() override { repaint(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListPanel)
};
