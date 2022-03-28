#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/NoteRollCommands.h"
#include "commands/SampleCommands.h"
#include "commands/TrackCommands.h"
#include "commands/TrackListCommands.h"
#include "commands/TransportCommands.h"
#include "common/listutil.h"
#include "ui/KeyboardControl.h"
#include "ui/MainWindow.h"

namespace trackman {

DesktopController::DesktopController(MainWindow &mainWindow, AudioDeviceManager &deviceManager)
    : mainWindow(mainWindow), deviceManager(deviceManager), applicationName(mainWindow.getName()),
      desktopComponent(*this), project(deviceManager), transportController(*this), mixerController(*this),
      trackListController(*this), instrumentsController(*this), previousTempo(project.getTempo()) {

    updateTitleBar();
}

void DesktopController::playbackStarted() {}

void DesktopController::playbackStopped() {}

void DesktopController::loopingChanged(bool shouldLoop) { project.getMixer().setLooping(shouldLoop); }

void DesktopController::recordingStarted() {
    auto selectedTrack = project.getTrackList().getSelectedTrack();
    if (selectedTrack != nullptr) {
        selectedTrack->startRecording();
        auto *midiRecorder = selectedTrack->getMidiRecorder();
        if (midiRecorder != nullptr) {
            midiRecorder->onMidiMessage = [this](const MidiMessage &message, double time) {
                midiMessageReceived(message, time);
            };
        }
        MessageManager::callAsync([this]() { trackListController.update(); });
    }
}

void DesktopController::recordingStopped() {
    auto selectedTrack = project.getTrackList().getSelectedTrack();
    if (selectedTrack != nullptr) {
        NoteRoll *noteRoll = selectedTrack->stopRecording();
        if (noteRoll != nullptr) {
            Command *command = new AddNoteRollCommand(*this, selectedTrack, noteRoll);
            commandList.pushCommand(command);
            dirty = true;
            updateTitleBar();
            desktopComponent.menuItemsChanged();
        }
        MessageManager::callAsync([this]() { trackListController.update(); });
    }
}

void DesktopController::recordingPaused() {
    auto selectedTrack = project.getTrackList().getSelectedTrack();
    if (selectedTrack != nullptr) {
        selectedTrack->pauseRecording();
    }
}

bool DesktopController::canRecord() { return project.getTrackList().canRecord(); }

void DesktopController::deleteNoteRoll(Track &track, NoteRoll *noteRoll) {
    trackListController.deleteNoteRoll(track, noteRoll);
    MessageManager::callAsync([this]() { instrumentsController.update(); });
}

void DesktopController::undeleteNoteRoll(Track &track, NoteRoll *noteRoll) {
    trackListController.undeleteNoteRoll(track, noteRoll);
    MessageManager::callAsync([this]() { instrumentsController.update(); });
}

void DesktopController::moveSelectedNoteRoll(
    NoteRoll &noteRoll, Track &fromTrack, Track *toTrack, double prevPos, double newPos) {
    Command *command = new MoveNoteRollCommand(*this, noteRoll, fromTrack, toTrack, prevPos, newPos);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::moveNoteRoll(NoteRoll &noteRoll, Track &fromTrack, Track &toTrack, double pos) {
    trackListController.moveNoteRoll(noteRoll, fromTrack, toTrack, pos);
    MessageManager::callAsync([this]() {
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::midiMessageReceived(const MidiMessage &message, double time) {
    trackListController.getTrackListPanel().resized();
}

void DesktopController::createKeyboard() {
    auto keyboard = new KeyboardControl(project.getKeyboardState());
    desktopComponent.createChildWindow("MIDI Keyboard", keyboard);
}

void DesktopController::prepareToPlay(int blockSize, double sampleRate) {
    project.getTransport().prepareToPlay(blockSize, sampleRate);
}

void DesktopController::releaseResources() { project.getTransport().releaseResources(); }

void DesktopController::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    project.getTransport().getNextAudioBlock(bufferToFill);
}

bool DesktopController::canUndo() const { return !commandList.isEmpty(); }

void DesktopController::undoLast() {
    commandList.undoLast();
    dirty = (commandList.peek() != saveCommand);
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::masterLevelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(mixerController, previousLevel);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::masterMuteToggled() {
    Command *command = new ToggleMasterMuteCommand(mixerController);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::tempoChanged(float newTempo) {
    Command *command = new ChangeTempoCommand(*this, previousTempo, newTempo);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::changeTempo(float newTempo) {
    previousTempo = newTempo;
    project.setTempo(newTempo);
    MessageManager::callAsync([this]() {
        trackListController.update();
        transportController.update();
        desktopComponent.repaint();
    });
}

void DesktopController::numeratorChanged(int newNumerator) {
    auto prevTimeSignature = project.getTimeSignature();
    auto newTimeSignature = TimeSignature(newNumerator, prevTimeSignature.getDenominator());
    Command *command = new ChangeTimeSignatureCommand(*this, prevTimeSignature, newTimeSignature);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::denominatorChanged(int newDenominator) {
    auto prevTimeSignature = project.getTimeSignature();
    auto newTimeSignature = TimeSignature(prevTimeSignature.getNumerator(), newDenominator);
    Command *command = new ChangeTimeSignatureCommand(*this, prevTimeSignature, newTimeSignature);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::changeTimeSignature(const TimeSignature &newTimeSignature) {
    project.setTimeSignature(newTimeSignature);
    MessageManager::callAsync([this]() {
        desktopComponent.repaint();
        transportController.update();
    });
}

void DesktopController::trackNameChanged(Track &track, String newName) {
    Command *command = new RenameTrackCommand(*this, track, newName);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::trackLevelChangeFinalized(Track &track, float previousLevel) {
    Command *command = new ChangeTrackVolumeCommand(mixerController, track, previousLevel);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::trackMuteToggled(Track &track) {
    Command *command = new ToggleMuteCommand(mixerController, track);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::trackSoloToggled(Track &track) {
    Command *command = new ToggleSoloCommand(mixerController, track);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::resize() { getTrackListController().getTrackListPanel().resize(); }

void DesktopController::addNewTrack() {
    Command *command = new AddTrackCommand(*this);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::addNewSample(Track *track, const File &file, int pos) {
    if (track != nullptr && track->hasMidi()) {
        return;
    }
    Command *command = new AddSampleCommand(*this, track, file, pos);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::moveSelectedSample(
    Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos) {
    Command *command = new MoveSampleCommand(*this, sample, fromTrack, toTrack, prevPos, newPos);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::resizeSample(Sample &sample, double prevLen, double newLen) {
    Command *command = new ResizeSampleCommand(trackListController, sample, prevLen, newLen);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::deleteSelected() {
    Track *selectedTrack = project.getTrackList().getSelectedTrack();
    if (selectedTrack != nullptr) {
        Sample *sample = project.getTrackList().getSelectedSample();
        if (sample != nullptr) {
            Command *command = new DeleteSampleCommand(*this, *selectedTrack, *sample);
            commandList.pushCommand(command);
            dirty = true;
            updateTitleBar();
            desktopComponent.menuItemsChanged();
            return;
        }
        NoteRoll *noteRoll = project.getTrackList().getSelectedNoteRoll();
        if (noteRoll != nullptr) {
            Command *command = new DeleteNoteRollCommand(*this, *selectedTrack, *noteRoll);
            commandList.pushCommand(command);
            dirty = true;
            updateTitleBar();
            desktopComponent.menuItemsChanged();
            return;
        }
        NativeMessageBox::showOkCancelBox(MessageBoxIconType::QuestionIcon, "",
            "Delete Track " + String(selectedTrack->getTrackNumber()) + "?", &desktopComponent,
            ModalCallbackFunction::create([this, selectedTrack](int result) {
                if (result > 0) {
                    Command *command = new DeleteTrackCommand(*this, selectedTrack);
                    commandList.pushCommand(command);
                    dirty = true;
                    updateTitleBar();
                    desktopComponent.menuItemsChanged();
                }
            }));
    }
}

String DesktopController::getSelectionType() const {
    if (project.getSelectedSample() != nullptr) {
        return "Sample";
    } else if (project.getSelectedNoteRoll() != nullptr) {
        return "Note Roll";
    } else if (project.getSelectedTrack() != nullptr) {
        return "Track";
    }
    return "";
}

Track *DesktopController::addTrack() {
    auto track = project.addTrack();
    MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
        instrumentsController.update();
    });
    return track;
}

void DesktopController::deleteTrack(Track *track, bool purge) {
    project.deleteTrack(track);
    if (purge) {
        project.getTrackList().removeTrack(track);
    }
    MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::undeleteTrack(Track *track) {
    project.getTrackList().undeleteTrack(track);
    MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::renameTrack(Track &track, const String &newName) {
    track.setName(newName);
    MessageManager::callAsync([this]() { mixerController.update(); });
}

Sample *DesktopController::addSample(Track &track, const File &file, int pos) {
    Sample *sample = trackListController.addSample(track, file, pos);
    MessageManager::callAsync([this]() {
        mixerController.update();
        instrumentsController.update();
    });
    return sample;
}

void DesktopController::moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos) {
    trackListController.moveSample(sample, fromTrack, toTrack, pos);
    MessageManager::callAsync([this]() {
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::deleteSample(Track &track, Sample *sample) {
    trackListController.deleteSample(track, sample);
    MessageManager::callAsync([this]() { instrumentsController.update(); });
}

void DesktopController::undeleteSample(Track &track, Sample *sample) {
    trackListController.undeleteSample(track, sample);
    MessageManager::callAsync([this]() { instrumentsController.update(); });
}

void DesktopController::saveProject(const function<void(bool saved)> &callback) {
    if (projectFile != File{}) {
        saveProjectFile(projectFile);
        if (callback) {
            callback(true);
        }
    } else {
        saveProjectAs(callback);
    }
}

void DesktopController::saveProjectAs(function<void(bool saved)> callback) {
    chooser = make_unique<FileChooser>("Save project as...", File{}, "*.trackman", true);
    auto chooserFlags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles;

    function<void(bool saved)> cb = callback;
    chooser->launchAsync(chooserFlags, [this, cb](const FileChooser &fc) {
        auto file = fc.getResult();
        if (file != File{}) {
            projectFile = file;
            saveProjectFile(file);
            if (cb)
                cb(true);
        } else {
            if (cb)
                cb(false);
        }
    });
}

void DesktopController::saveProjectFile(const File &file) {
    string json = project.to_json();
    TemporaryFile tempFile(file);
    FileOutputStream output(tempFile.getFile());

    if (!output.openedOk()) {
        DBG("FileOutputStream didn't open correctly ...");
        // ... some other error handling
        return;
    }

    output.writeText(String(json), false, false, nullptr);
    output.flush();

    if (output.getStatus().failed()) {
        DBG("An error occurred in the FileOutputStream");
        // ... some other error handling
        return;
    }

    tempFile.overwriteTargetFileWithTemporary();
    saveCommand = commandList.peek();
    dirty = false;
    updateTitleBar();
    desktopComponent.menuItemsChanged();
}

void DesktopController::openProject() {
    chooser = make_unique<FileChooser>("Load project...", File{}, "*.trackman", true);
    auto chooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const FileChooser &fc) {
        auto file = fc.getResult();
        if (file != File{}) {
            projectFile = file;
            project.from_json(
                mainWindow.getMainAudioComponent().getFormatManager(), file.getFullPathName().toStdString());
            previousTempo = project.getTempo();
            MessageManager::callAsync([this]() {
                trackListController.update();
                transportController.update();
                mixerController.update();
                instrumentsController.update();
            });
            commandList.clear();
            saveCommand = nullptr;
            dirty = false;
            updateTitleBar();
            desktopComponent.menuItemsChanged();
        }
    });
}

void DesktopController::exportProject() {
    chooser = make_unique<FileChooser>("Save project as...", File{}, "*.wav", true);
    auto chooserFlags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const FileChooser &fc) {
        auto file = fc.getResult();
        if (file != File{}) {
            project.writeAudioFile(file);
        }
    });
}

void DesktopController::updateTitleBar() {
    mainWindow.setName((projectFile != File{} ? projectFile.getFileNameWithoutExtension() + (dirty ? " [modified]" : "")
                                              : "[untitled]") +
                       " - " + applicationName);
}

void DesktopController::selectionChanged(Track *track) {
    project.getTrackList().setSelected(track);
    MessageManager::callAsync([this, track]() {
        transportController.selectionChanged(track);
        trackListController.repaint();
        mixerController.repaint();
        instrumentsController.repaint();
    });
    desktopComponent.menuItemsChanged();
}

void DesktopController::fileDragEnter(const StringArray &files, int x, int y) {
    trackListController.fileDragEnter(files, x, y);
}

void DesktopController::fileDragMove(const StringArray &files, int x, int y) {
    trackListController.fileDragMove(files, x, y);
}

void DesktopController::fileDragExit(const StringArray &files) { trackListController.fileDragExit(files); }

void DesktopController::filesDropped(const StringArray &files, int x, int y) {
    trackListController.filesDropped(files, x, y);
}

void DesktopController::verticalScaleIncreased() {
    project.incrementVerticalScale();
    MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::verticalScaleDecreased() {
    project.decrementVerticalScale();
    MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleIncreased() {
    project.incrementHorizontalScale();
    MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleDecreased() {
    project.decrementHorizontalScale();
    MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

} // namespace trackman
