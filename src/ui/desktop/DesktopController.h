#pragma once

#include "DesktopComponent.h"
#include "FileDragDropTarget.h"
#include "TrackScaleListener.h"
#include "model/MidiRecorder.h"
#include "commands/CommandList.h"
#include "model/Project.h"
#include "model/TrackList.h"
#include "ui/instruments/InstrumentsController.h"
#include "ui/mixer/MixerController.h"
#include "ui/mixer/TrackControlListener.h"
#include "ui/tracks/TrackListController.h"
#include "ui/transport/TransportController.h"

using namespace std;
using namespace juce;

namespace trackman {

class MainWindow;

// TODO: consider using FileBasedDocument
class DesktopController : public AudioSource,
                          public FileDragDropTarget,
                          public MasterTrackListener,
                          public TrackControlListener,
                          public TrackScaleListener {
  public:
    DesktopController(MainWindow &mainWindow, AudioDeviceManager &deviceManager);
    ~DesktopController() override = default;

    MainWindow &getMainWindow() { return mainWindow; }
    DesktopComponent &getDesktopComponent() { return desktopComponent; }
    Project &getProject() { return project; }
    AudioDeviceManager &getDeviceManager() { return deviceManager; }

    MixerController &getMixerController() { return mixerController; }
    Mixer &getMixer() { return project.getMixer(); }
    void midiMessageReceived(const MidiMessage &message, double time);

    TransportController &getTransportController() { return transportController; }
    TrackListController &getTrackListController() { return trackListController; }
    InstrumentsController &getInstrumentsController() { return instrumentsController; };

    bool canUndo() const;
    void undoLast();
    String getLastCommandName() const { return commandList.getLastCommandName(); }
    bool hasSelection() const { return getSelectionType() != ""; }
    bool isDirty() const { return dirty; }

    void resize();

    void playbackStarted();
    void playbackStopped();
    void loopingChanged(bool shouldLoop);
    void recordingStarted();
    void recordingStopped();
    void recordingPaused();
    bool canRecord();

    void createKeyboard();

    void addNewTrack();
    void addNewSample(Track *track, const File &file, int pos);
    void moveSelectedSample(Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos);
    void resizeSample(Sample &sample, double prevLen, double newLen);
    void deleteSelected();

    void selectionChanged(Track *track);
    String getSelectionType() const;

    Track *addTrack();
    void deleteTrack(Track *track, bool purge);
    void undeleteTrack(Track *track);
    void renameTrack(Track &track, const String &newName);

    Sample *addSample(Track &track, const File &file, int pos);
    void moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos);
    void deleteSample(Track &track, Sample *sample);
    void undeleteSample(Track &track, Sample *sample);

    void saveProject(const function<void(bool saved)> &callback = nullptr);
    void saveProjectAs(function<void(bool saved)> callback = nullptr);
    void openProject();
    void exportProject();

    void tempoChanged(float newTempo);
    void numeratorChanged(int newNumerator);
    void denominatorChanged(int newDenominator);

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // FileDragDropTarget
    void fileDragEnter(const StringArray &files, int x, int y) override;
    void fileDragMove(const StringArray &files, int x, int y) override;
    void fileDragExit(const StringArray &files) override;
    void filesDropped(const StringArray &files, int x, int y) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChangeFinalized(float previousLevel) override;
    void masterMuteToggled() override;

    //==============================================================================
    // TrackControlListener
    void trackNameChanged(Track &track, String newName) override;
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
    AudioDeviceManager &deviceManager;
    String applicationName;

    CommandList commandList;
    Project project;
    TransportController transportController;
    MixerController mixerController;
    TrackListController trackListController;
    InstrumentsController instrumentsController;

    DesktopComponent desktopComponent;

    unique_ptr<FileChooser> chooser;
    File projectFile;
    bool dirty = false;
    Command *saveCommand = nullptr;

    float previousTempo;

    void saveProjectFile(const File &file);
    void updateTitleBar();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DesktopController)
};

} // namespace trackman
