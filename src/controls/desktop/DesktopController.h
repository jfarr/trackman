#pragma once

#include "FileDragDropTarget.h"
#include "TrackListListener.h"
#include "TrackScaleListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/TrackControlListener.h"
#include "controls/tracks/TrackListController.h"
#include "model/Project.h"
#include "model/TrackList.h"

class DesktopComponent;

// TODO: consider using juce::FileBasedDocument
class DesktopController : public FileDragDropTarget,
                          public MasterTrackListener,
                          public TrackControlListener,
                          public TrackScaleListener {
  public:
    DesktopController(juce::DocumentWindow &mainWindow, DesktopComponent &desktopComponent, juce::AudioDeviceManager &deviceManager,
        juce::AudioFormatManager &formatManager);
    ~DesktopController() override = default;

    Project &getProject() { return project; }
    TrackList &getTrackList() { return trackList; }
    Mixer &getMixer() { return mixer; }
    MixerController &getMixerController() { return mixerController; }
    TrackListController &getTrackListController() { return trackListController; }

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }
    bool hasSelection() const { return getSelectionType() != ""; }

    void resize();

    void addNewTrack();
    void addNewSample(Track *track, juce::File file, int pos);
    void moveSelectedSample(Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos);
    void resizeSample(Sample &sample, double prevLen, double newLen);
    void deleteSelected();

    void selectionChanged(Track *track);
    juce::String getSelectionType() const;

    Track *addTrack();
    void deleteTrack(Track *track, bool purge);
    void undeleteTrack(Track *track);
    void renameTrack(Track &track, juce::String newName);

    Sample *addSample(Track &track, juce::File file, int pos);
    void deleteSample(Track &track, Sample *sample);

    void saveProject();
    void saveProjectAs();
    void openProject();
    void exportProject();

    //==============================================================================
    // FileDragDropTarget
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragMove(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChangeFinalized(float previousLevel) override;
    void masterMuteToggled() override;

    //==============================================================================
    // TrackControlListener
    void trackNameChanged(Track &track, juce::String newName) override;
    void trackLevelChangeFinalized(Track &track, float previousLevel) override;
    void trackMuteToggled(Track &track) override;
    void trackSoloToggled(Track &track) override;

    //==============================================================================
    // TrackScaleListener
    void scaleIncreased() override;
    void scaleDecreased() override;


  private:
    CommandList commandList;
    TrackList trackList;
    Mixer mixer;
    Project project;
    MixerController mixerController;
    TrackListController trackListController;

    std::unique_ptr<juce::FileChooser> chooser;
    juce::File projectFile;
    bool dirty = false;
    Command *saveCommand = nullptr;
    DesktopComponent &desktopComponent;
    juce::DocumentWindow &mainWindow;
    juce::String applicationName;

    juce::AudioDeviceManager &deviceManager;
    juce::AudioFormatManager &formatManager;

    void saveProjectFile(juce::File file);
    void updateTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopController)
};
