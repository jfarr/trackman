#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "TrackListPanel.h"
#include "controls/desktop/TrackListListener.h"
#include "model/TrackList.h"

class TrackListController : public TrackListListener {
  public:
    TrackListController(TrackList &trackList);
    ~TrackListController() {}

    TrackListPanel &getTrackListPanel() { return trackListPanel; }
    juce::Component &getViewport() { return trackListViewport; }

    void update();
    void repaint();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped (const juce::StringArray& files, int x, int y);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track) override;

  private:
    TrackList &trackList;
    TrackListPanel trackListPanel;
    juce::Viewport trackListViewport;
    float scale = 25;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
    std::list<TrackListListener *> listeners;

    void notifySelectionChanged(Track &track);
    void updateLane(Track &track);
    TrackLaneController *getLane(Track &track);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListController)
};
