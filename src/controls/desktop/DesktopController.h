#pragma once

#include "FileDragDropTarget.h"
#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MasterTrackListener.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/TrackControlListener.h"
#include "controls/tracks/TrackListController.h"
#include "model/Project.h"
#include "model/TrackList.h"

// TODO: consider using juce::FileBasedDocument
class DesktopController : public MasterTrackListener,
                          public TrackControlListener,
                          public TrackListListener,
                          public SampleListener,
                          public FileDragDropTarget {
  public:
    DesktopController(juce::DocumentWindow &mainWindow, juce::AudioDeviceManager &deviceManager,
        juce::AudioFormatManager &formatManager);
    ~DesktopController();

    Mixer &getMixer() { return mixer; }
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

    //==============================================================================
    // SampleListener
    void sampleAdded(Track &track, juce::File file, int pos) override;

    //==============================================================================
    // FileDragDropTarget
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragMove(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

  private:
    CommandList commandList;
    // TODO: move trackList and mixer inside project
    TrackList trackList;
    Mixer mixer;
    MixerController mixerController;
    TrackListController trackListController;

    Project project;
    std::unique_ptr<juce::FileChooser> chooser;
    juce::File projectFile;
    bool dirty = false;
    Command *saveCommand = nullptr;
    juce::DocumentWindow &mainWindow;
    juce::String applicationName;

    juce::AudioDeviceManager &deviceManager;
    juce::AudioFormatManager &formatManager;

    void saveProjectFile(juce::File file);
    void updateTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopController)
};
