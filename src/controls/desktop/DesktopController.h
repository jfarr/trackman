#pragma once

#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MasterTrackListener.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/TrackControlListener.h"
#include "controls/tracks/TrackListController.h"
#include "model/TrackList.h"
#include "model/Project.h"

class DesktopController : public MasterTrackListener, public TrackControlListener, public TrackListListener {
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

    void saveProject();

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChangeFinalized(float previousLevel) override;
    void masterMuteToggled() override;

    //==============================================================================
    // TrackControlListener
    void levelChangeFinalized(Track &track, float previousLevel) override;
    void muteToggled(Track &track) override;

  private:
    CommandList commandList;
    TrackList trackList;
    MixerController mixerController;
    TrackListController trackListController;

    Project project;
    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager &formatManager;
};
