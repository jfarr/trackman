#pragma once

#include <JuceHeader.h>

#include "TrackLaneControl.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Track.h"

class TrackListController;

class TrackLaneController : public juce::MouseListener, public TrackListListener {
  public:
    TrackLaneController(TrackListController &trackListController, Track &track, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
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
    TrackListController &trackListController;
    Track &track;
    juce::AudioTransportSource &transport;
    juce::AudioFormatManager &formatManager;

    TrackLaneControl trackLaneControl;
    std::list<std::unique_ptr<SampleThumbnail>> thumbnails;
    std::list<TrackListListener *> trackListListeners;

    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneController)
};
