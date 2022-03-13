#pragma once

#include <JuceHeader.h>

#include "TrackLaneControl.h"
#include "model/Track.h"
#include "ui/desktop/TrackListListener.h"

class TrackListController;

class TrackLaneController : public juce::MouseListener, public TrackListListener {
  public:
    TrackLaneController(Project &project, Track &track, TrackListController &trackListController,
        juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
    ~TrackLaneController();

    Track &getTrack() { return track; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void update();
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

  private:
    Project &project;
    Track &track;
    juce::AudioTransportSource &transport;
    juce::AudioFormatManager &formatManager;
    TrackListController &trackListController;

    TrackLaneControl trackLaneControl;
    std::list<std::unique_ptr<SampleThumbnail>> thumbnails;
    std::list<TrackListListener *> trackListListeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneController)
};
