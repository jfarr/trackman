#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "TrackListPanel.h"
#include "TrackListViewport.h"
#include "model/Project.h"
#include "ui/desktop/TrackListListener.h"
#include "ui/tracks/SampleListener.h"

using namespace std;
using namespace juce;

namespace trackman {

class DesktopController;

class TrackListController : public TrackListListener, public SampleListener {
  public:
    TrackListController(DesktopController &desktopController);
    ~TrackListController();

    TrackListPanel &getTrackListPanel() { return trackListPanel; }
    TrackListViewport &getViewport() { return trackListViewport; }

    void update();
    void repaint();

    Sample *addSample(Track &track, File file, int pos);
    void moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos);
    void resizeSample(Sample &sample, double length);
    void sampleSelected(Track &track, Sample &sample);
    void deleteSample(Track &track, Sample *sample);
    void undeleteSample(Track &track, Sample *sample);
    void sampleDragged(SampleThumbnail &thumbnail, int x, int screenY);

    void fileDragEnter(const StringArray &files, int x, int y);
    void fileDragMove(const StringArray &files, int x, int y);
    void fileDragExit(const StringArray &files);
    void filesDropped(const StringArray &files, int x, int y);

    void noteRollSelected(Track &track, NoteRoll &noteRoll);
    void deleteNoteRoll(Track &track, NoteRoll *noteRoll);
    void undeleteNoteRoll(Track &track, NoteRoll *noteRoll);
    void noteRollDragged(NoteCanvas &thumbnail, int x, int screenY);
    void noteRollMoved(Track &track, NoteRoll &noteRoll, int x, int y);
    void moveNoteRoll(NoteRoll &noteRoll, Track &fromTrack, Track &toTrack, double pos);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

    //==============================================================================
    // SampleListener
    void sampleMoved(Track &track, Sample &sample, int x, int y) override;
    void sampleResized(Sample &sample, int width) override;
    void dragEnded() override;

  private:
    DesktopController &desktopController;
    Project &project;

    TrackListViewport trackListViewport;
    TrackListPanel trackListPanel;
    list<unique_ptr<TrackLaneController>> lanes;
    Track *selected = nullptr;
    bool selectingSample = false;
    Track *currentDragTrack = nullptr;
    TrackLaneController *newDragLane = nullptr;

    bool selectingNoteRoll = false;

    void updateLane(Track &track);
    TrackLaneController *getLane(Track &track);
    void updateLanes();
    void removeDragLane();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListController)
};

} // namespace trackman
