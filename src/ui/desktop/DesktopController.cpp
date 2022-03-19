#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackCommands.h"
#include "commands/TrackListCommands.h"
#include "common/listutil.h"
#include "ui/KeyboardControl.h"
#include "ui/MainWindow.h"

DesktopController::DesktopController(MainWindow &mainWindow, juce::AudioDeviceManager &deviceManager)
    : mainWindow(mainWindow), deviceManager(deviceManager), applicationName(mainWindow.getName()),
      desktopComponent(*this), project(deviceManager, midiRecorder), mixerController(*this), trackListController(*this),
      instrumentsController(*this), midiRecorder(project, deviceManager) {

    getMixer().addSource(&midiRecorder);
    updateTitleBar();
}

void DesktopController::createKeyboard() {
    auto keyboard = new KeyboardControl(midiRecorder.getKeyboardState());
    desktopComponent.createChildWindow("MIDI Keyboard", keyboard);
}

void DesktopController::prepareToPlay(int blockSize, double sampleRate) {
    project.getMixer().prepareToPlay(blockSize, sampleRate);
}

void DesktopController::releaseResources() { project.getMixer().releaseResources(); }

void DesktopController::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    project.getMixer().getNextAudioBlock(bufferToFill);
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

void DesktopController::trackNameChanged(Track &track, juce::String newName) {
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

void DesktopController::addNewSample(Track *track, const juce::File &file, int pos) {
    if (track->hasMidi()) {
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
    Track *track = project.getTrackList().getSelectedTrack();
    Sample *sample = project.getTrackList().getSelectedSample();
    if (track != nullptr && sample != nullptr) {
        Command *command = new DeleteSampleCommand(*this, *track, *sample);
        commandList.pushCommand(command);
        dirty = true;
        updateTitleBar();
        desktopComponent.menuItemsChanged();
        return;
    }
    Track *selected = project.getTrackList().getSelectedTrack();
    if (selected != nullptr) {
        juce::NativeMessageBox::showOkCancelBox(juce::MessageBoxIconType::QuestionIcon, "",
            "Delete Track " + juce::String(selected->getTrackNumber()) + "?", &desktopComponent,
            juce::ModalCallbackFunction::create([this, selected](int result) {
                if (result > 0) {
                    Command *command = new DeleteTrackCommand(*this, selected);
                    commandList.pushCommand(command);
                    dirty = true;
                    updateTitleBar();
                    desktopComponent.menuItemsChanged();
                }
            }));
    }
    desktopComponent.menuItemsChanged();
}

juce::String DesktopController::getSelectionType() const {
    if (project.getSelectedSample() != nullptr) {
        return "sample";
    } else if (project.getSelectedTrack() != nullptr) {
        return "track";
    }
    return "";
}

Track *DesktopController::addTrack() {
    auto track = project.addTrack();
    juce::MessageManager::callAsync([this]() {
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
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::undeleteTrack(Track *track) {
    project.getTrackList().undeleteTrack(track);
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::renameTrack(Track &track, const juce::String &newName) {
    track.setName(newName);
    juce::MessageManager::callAsync([this]() { mixerController.update(); });
}

Sample *DesktopController::addSample(Track &track, const juce::File &file, int pos) {
    Sample *sample = trackListController.addSample(track, file, pos);
    juce::MessageManager::callAsync([this]() {
        mixerController.update();
        instrumentsController.update();
    });
    return sample;
}

void DesktopController::moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos) {
    trackListController.moveSample(sample, fromTrack, toTrack, pos);
    juce::MessageManager::callAsync([this]() {
        mixerController.update();
        instrumentsController.update();
    });
}

void DesktopController::deleteSample(Track &track, Sample *sample) {
    trackListController.deleteSample(track, sample);
    juce::MessageManager::callAsync([this]() {
        instrumentsController.update();
    });
}

void DesktopController::undeleteSample(Track &track, Sample *sample) {
    trackListController.undeleteSample(track, sample);
    juce::MessageManager::callAsync([this]() {
        instrumentsController.update();
    });

}

void DesktopController::saveProject(const std::function<void(bool saved)> &callback) {
    if (projectFile != juce::File{}) {
        saveProjectFile(projectFile);
        if (callback) {
            callback(true);
        }
    } else {
        saveProjectAs(callback);
    }
}

void DesktopController::saveProjectAs(std::function<void(bool saved)> callback) {
    chooser = std::make_unique<juce::FileChooser>("Save project as...", juce::File{}, "*.trackman", true);
    auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    std::function<void(bool saved)> cb = callback;
    chooser->launchAsync(chooserFlags, [this, cb](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
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

void DesktopController::saveProjectFile(const juce::File &file) {
    std::string json = project.to_json();
    juce::TemporaryFile tempFile(file);
    juce::FileOutputStream output(tempFile.getFile());

    if (!output.openedOk()) {
        DBG("FileOutputStream didn't open correctly ...");
        // ... some other error handling
        return;
    }

    output.writeText(juce::String(json), false, false, nullptr);
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
    chooser = std::make_unique<juce::FileChooser>("Load project...", juce::File{}, "*.trackman", true);
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            projectFile = file;
            project.from_json(
                mainWindow.getMainAudioComponent().getFormatManager(), file.getFullPathName().toStdString());
            juce::MessageManager::callAsync([this]() {
                trackListController.update();
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
    chooser = std::make_unique<juce::FileChooser>("Save project as...", juce::File{}, "*.wav", true);
    auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            project.writeAudioFile(file);
        }
    });
}

void DesktopController::updateTitleBar() {
    mainWindow.setName(
        (projectFile != juce::File{} ? projectFile.getFileNameWithoutExtension() + (dirty ? " [modified]" : "")
                                     : "[untitled]") +
        " - " + applicationName);
}

void DesktopController::recordingStopped() {
    auto selected = project.getTrackList().getSelectedTrack();
    if (selected != nullptr) {
        selected->stopRecording();
//        auto messages = midiRecorder.getMidiMessages();
//        messages.updateMatchedPairs();
//        selected->setMidiMessages(messages);
        juce::MessageManager::callAsync([this]() {
            trackListController.update();
        });
    }
}

void DesktopController::selectionChanged(Track *track) {
    project.getTrackList().setSelected(track);
    juce::MessageManager::callAsync([this, track]() {
//        midiRecorder.reset();
//        midiRecorder.setMidiMessages(track == nullptr ? juce::MidiMessageSequence() : track->getMidiMessages());
        mixerController.getMixerPanel().getTransportControl().selectionChanged(track);
        trackListController.repaint();
        mixerController.repaint();
        instrumentsController.repaint();
    });
    desktopComponent.menuItemsChanged();
}

void DesktopController::fileDragEnter(const juce::StringArray &files, int x, int y) {
    trackListController.fileDragEnter(files, x, y);
}

void DesktopController::fileDragMove(const juce::StringArray &files, int x, int y) {
    trackListController.fileDragMove(files, x, y);
}

void DesktopController::fileDragExit(const juce::StringArray &files) { trackListController.fileDragExit(files); }

void DesktopController::filesDropped(const juce::StringArray &files, int x, int y) {
    trackListController.filesDropped(files, x, y);
}

void DesktopController::verticalScaleIncreased() {
    project.incrementVerticalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::verticalScaleDecreased() {
    project.decrementVerticalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleIncreased() {
    project.incrementHorizontalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleDecreased() {
    project.decrementHorizontalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        instrumentsController.update();
        desktopComponent.resized();
    });
}
