#pragma once

#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MasterTrackListener.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/TrackControlListener.h"
#include "controls/tracks/TrackListController.h"
#include "model/Project.h"
#include "model/TrackList.h"

// TODO: consider using juce::FileBasedDocument
class DesktopController : public MasterTrackListener, public TrackControlListener, public TrackListListener {
  public:
    DesktopController(juce::DocumentWindow& mainWindow, juce::AudioFormatManager &formatManager);
    ~DesktopController();

    TrackList &getTrackList() { return trackList; }
    MixerController &getMixerController() { return mixerController; }
    TrackListController &getTrackListController() { return trackListController; }

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }
    bool hasSelection() const { return trackList.hasSelection(); }

    void resize();

    void addNewTrack();
    void deleteSelectedTrack();

    Track *addTrack(juce::String name);
    void deleteTrack(Track *track, bool purge);
    void undeleteTrack(Track *track);

    void saveProject();
    void saveProjectAs();
    void openProject();

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
    juce::File projectFile;
    bool dirty = false;
    Command *saveCommand = nullptr;
    juce::DocumentWindow &mainWindow;
    juce::String applicationName;

    juce::AudioFormatManager &formatManager;

    void saveProjectFile(juce::File file);
    void updateTitleBar();
};
