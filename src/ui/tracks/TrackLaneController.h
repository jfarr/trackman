#pragma once

#include <JuceHeader.h>

#include "TrackLaneControl.h"
#include "model/Track.h"
#include "ui/desktop/TrackListListener.h"

using namespace std;
using namespace juce;

namespace trackman {

class TrackListController;

class TrackLaneController : public MouseListener, public TrackListListener {
  public:
    TrackLaneController(Project &project, Track &track, TrackListController &trackListController,
        AudioTransportSource &transport, AudioFormatManager &formatManager);
    ~TrackLaneController();

    Track &getTrack() { return track; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void update();
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MouseListener
    void mouseDown(const MouseEvent &event) override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

  private:
    Project &project;
    Track &track;
    AudioTransportSource &transport;
    AudioFormatManager &formatManager;
    TrackListController &trackListController;

    TrackLaneControl trackLaneControl;
    list<unique_ptr<SampleThumbnail>> sampleThumbnails;
    list<unique_ptr<NoteCanvas>> noteCanvases;

    list<TrackListListener *> trackListListeners;

    void updateSamples();
    void updateMidi();
    void notifySelectionChanged();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneController)
};

} // namespace trackman
