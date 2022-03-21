#pragma once

#include "DesktopComponent.h"
#include "FileDragDropTarget.h"
#include "TrackListListener.h"
#include "TrackScaleListener.h"
#include "audio/MidiRecorder.h"
#include "commands/CommandList.h"
#include "model/Project.h"
#include "model/TrackList.h"
#include "ui/instruments/InstrumentsController.h"
#include "ui/mixer/MixerController.h"
#include "ui/mixer/TrackControlListener.h"
#include "ui/tracks/TrackListController.h"
#include "ui/transport/TransportController.h"

class MainWindow;

// TODO: consider using juce::FileBasedDocument
class DesktopController : public juce::AudioSource,
                          public FileDragDropTarget,
                          public MasterTrackListener,
                          public TrackControlListener,
                          public TrackScaleListener {
  public:
    DesktopController(MainWindow &mainWindow, juce::AudioDeviceManager &deviceManager);
    ~DesktopController() override = default;

    MainWindow &getMainWindow() { return mainWindow; }
    DesktopComponent &getDesktopComponent() { return desktopComponent; }
    Project &getProject() { return project; }
    juce::AudioDeviceManager &getDeviceManager() { return deviceManager; }

    MixerController &getMixerController() { return mixerController; }
    Mixer &getMixer() { return project.getMixer(); }
    MidiRecorder &getMidiRecorder() { return midiRecorder; }

    trackman::TransportController &getTransportController() { return transportController; }
    TrackListController &getTrackListController() { return trackListController; }
    InstrumentsController &getInstrumentsController() { return instrumentsController; };

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }
    bool hasSelection() const { return getSelectionType() != ""; }
    bool isDirty() const { return dirty; }

    void resize();

    void loopingChanged(bool shouldLoop);

    void createKeyboard();

    void addNewTrack();
    void addNewSample(Track *track, const juce::File &file, int pos);
    void moveSelectedSample(Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos);
    void resizeSample(Sample &sample, double prevLen, double newLen);
    void deleteSelected();

    void recordingStopped();
    void selectionChanged(Track *track);
    juce::String getSelectionType() const;

    Track *addTrack();
    void deleteTrack(Track *track, bool purge);
    void undeleteTrack(Track *track);
    void renameTrack(Track &track, const juce::String &newName);

    Sample *addSample(Track &track, const juce::File &file, int pos);
    void moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos);
    void deleteSample(Track &track, Sample *sample);
    void undeleteSample(Track &track, Sample *sample);

    void saveProject(const std::function<void(bool saved)> &callback = nullptr);
    void saveProjectAs(std::function<void(bool saved)> callback = nullptr);
    void openProject();
    void exportProject();

    void tempoChanged(float newTempo);
    void numeratorChanged(int newNumerator);
    void denominatorChanged(int newDenominator);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

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
    void verticalScaleIncreased() override;
    void verticalScaleDecreased() override;
    void horizontalScaleIncreased() override;
    void horizontalScaleDecreased() override;

  private:
    MainWindow &mainWindow;
    juce::AudioDeviceManager &deviceManager;
    juce::String applicationName;

    CommandList commandList;
    Project project;
    trackman::TransportController transportController;
    MixerController mixerController;
    TrackListController trackListController;
    InstrumentsController instrumentsController;

    DesktopComponent desktopComponent;

    MidiRecorder midiRecorder;

    std::unique_ptr<juce::FileChooser> chooser;
    juce::File projectFile;
    bool dirty = false;
    Command *saveCommand = nullptr;

    float previousTempo;

    void saveProjectFile(const juce::File &file);
    void updateTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopController)
};
