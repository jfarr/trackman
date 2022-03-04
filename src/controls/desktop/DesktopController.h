#pragma once

#include "TrackControllerListener.h"
#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/MixerPanel.h"
#include "controls/mixer/TrackControlListener.h"
#include "controls/mixer/TrackController.h"
#include "controls/tracks/TrackListController.h"
#include "controls/tracks/TrackListPanel.h"
#include "model/TrackList.h"

class DesktopController : /*public MixerPanelListener,*/
                          public MasterTrackListener,
                          public TrackControlListener,
                          //                          public TrackControllerListener,
                          public TrackListListener {
  public:
    DesktopController(juce::AudioFormatManager &formatManager);
    ~DesktopController();

    TrackList &getTrackList() { return trackList; }
    MixerController &getMixerController() { return mixerController; }
    TrackListController &getTrackListController() { return trackListController; }

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }

    void addNewTrack();
    void deleteSelectedTrack();

    Track *addTrack(juce::String name);
    void deleteTrack(Track *track, bool purge);
    void undeleteTrack(Track *track);

    //    void addTrackController(TrackController *track);
    //    void removeTrackController(TrackController *track);

    //==============================================================================
    // MixerPanelListener
    //    void mixerResized(juce::Rectangle<int> area) override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track, juce::Component *source) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChangeFinalized(float previousLevel) override;

    //==============================================================================
    // TrackControlListener
    void levelChangeFinalized(TrackControl &trackControl, float previousLevel) override;

    //==============================================================================
    // TrackControllerListener
    //    void selectionChanged(TrackController *newSelected) override;

  private:
    CommandList commandList;
    TrackList trackList;
    //    std::list<TrackController *> tracks;
    //    TrackController *selected = nullptr;
    MixerController mixerController;
    TrackListController trackListController;

    juce::AudioFormatManager &formatManager;
    //    TrackListPanel &trackListPanel;
};
