#include "DesktopController.h"
#include "DesktopComponent.h"
#include "commands/MixerCommands.h"
#include "commands/TrackCommands.h"
#include "commands/TrackListCommands.h"
#include "common/listutil.h"

DesktopController::DesktopController(juce::DocumentWindow &mainWindow, DesktopComponent &desktopComponent,
    juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager)
    : mixer(deviceManager.getAudioDeviceSetup().sampleRate), project(trackList, mixer),
      mixerController(*this, formatManager),
      trackListController(*this, mixerController.getMixer().getTransportSource(), deviceManager, formatManager),
      mainWindow(mainWindow), desktopComponent(desktopComponent), applicationName(mainWindow.getName()),
      deviceManager(deviceManager), formatManager(formatManager) {
    updateTitleBar();
}

bool DesktopController::canUndo() const { return !commandList.isEmpty(); }

void DesktopController::undoLast() {
    commandList.undoLast();
    dirty = (commandList.peek() != saveCommand);
    updateTitleBar();
}

void DesktopController::masterLevelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(mixerController, previousLevel);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::masterMuteToggled() {
    Command *command = new ToggleMasterMuteCommand(mixerController);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::trackNameChanged(Track &track, juce::String newName) {
    Command *command = new RenameTrackCommand(*this, track, newName);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::trackLevelChangeFinalized(Track &track, float previousLevel) {
    Command *command = new ChangeTrackVolumeCommand(mixerController, track, previousLevel);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::trackMuteToggled(Track &track) {
    Command *command = new ToggleMuteCommand(mixerController, track);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::trackSoloToggled(Track &track) {
    Command *command = new ToggleSoloCommand(mixerController, track);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::resize() { getTrackListController().getTrackListPanel().resize(); }

void DesktopController::addNewTrack() {
    Command *command = new AddTrackCommand(*this);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::addNewSample(Track *track, juce::File file, int pos) {
    Command *command = new AddSampleCommand(*this, track, file, pos);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::moveSelectedSample(
    Sample &sample, Track &fromTrack, Track *toTrack, double prevPos, double newPos) {
    Command *command = new MoveSampleCommand(*this, sample, fromTrack, toTrack, prevPos, newPos);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::resizeSample(Sample &sample, double prevLen, double newLen) {
    Command *command = new ResizeSampleCommand(trackListController, sample, prevLen, newLen);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::deleteSelected() {
    Track *track = trackList.getSelected();
    Sample *sample = trackList.getSelectedSample();
    if (track != nullptr && sample != nullptr) {
        Command *command = new DeleteSampleCommand(trackListController, *track, *sample);
        commandList.pushCommand(command);
        dirty = true;
        updateTitleBar();
        return;
    }
    Track *selected = trackList.getSelected();
    if (selected != nullptr) {
        juce::NativeMessageBox::showOkCancelBox(juce::MessageBoxIconType::QuestionIcon, "",
            "Delete Track " + juce::String(selected->getNumber()) + "?", &desktopComponent,
            juce::ModalCallbackFunction::create([this, selected](int result) {
                if (result > 0) {
                    Command *command = new DeleteTrackCommand(*this, selected);
                    commandList.pushCommand(command);
                    dirty = true;
                    updateTitleBar();
                }
            }));
    }
}

juce::String DesktopController::getSelectionType() const {
    if (trackList.getSelectedSample() != nullptr) {
        return "sample";
    } else if (trackList.getSelected() != nullptr) {
        return "track";
    }
    return "";
}

Track *DesktopController::addTrack() {
    auto track = trackList.addTrack();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
    });
    return track;
}

void DesktopController::deleteTrack(Track *track, bool purge) {
    trackList.deleteTrack(track);
    if (purge) {
        trackList.removeTrack(track);
    }
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
    });
}

void DesktopController::undeleteTrack(Track *track) {
    trackList.undeleteTrack(track);
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        mixerController.update();
    });
}

void DesktopController::renameTrack(Track &track, juce::String newName) {
    track.setName(newName);
    juce::MessageManager::callAsync([this]() { mixerController.update(); });
}

Sample *DesktopController::addSample(Track &track, juce::File file, int pos) {
    Sample *sample = trackListController.addSample(track, file, pos);
    juce::MessageManager::callAsync([this]() { mixerController.update(); });
    return sample;
}

void DesktopController::deleteSample(Track &track, Sample *sample) { trackListController.deleteSample(track, sample); }

void DesktopController::saveProject() {
    if (projectFile != juce::File{}) {
        saveProjectFile(projectFile);
    } else {
        saveProjectAs();
    }
}

void DesktopController::saveProjectAs() {
    chooser = std::make_unique<juce::FileChooser>("Save project as...", juce::File{}, "*.trackman", true);
    auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            projectFile = file;
            saveProjectFile(file);
        }
    });
}

void DesktopController::saveProjectFile(juce::File file) {
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
}

void DesktopController::openProject() {
    chooser = std::make_unique<juce::FileChooser>("Load project...", juce::File{}, "*.trackman", true);
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            projectFile = file;
            project.from_json(deviceManager, formatManager, file.getFullPathName().toStdString());
            trackListController.update();
            mixerController.update();
            commandList.clear();
            saveCommand = nullptr;
            dirty = false;
            updateTitleBar();
        }
    });
}

void DesktopController::exportProject() {
    chooser = std::make_unique<juce::FileChooser>("Save project as...", juce::File{}, "*.wav", true);
    auto chooserFlags = juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            trackList.writeAudioFile(file, mixer.getSource(), deviceManager.getAudioDeviceSetup().sampleRate, 16);
        }
    });
}

void DesktopController::updateTitleBar() {
    mainWindow.setName(
        (projectFile != juce::File{} ? projectFile.getFileNameWithoutExtension() + (dirty ? " [modified]" : "")
                                     : "[untitled]") +
        " - " + applicationName);
}

void DesktopController::selectionChanged(Track *track) {
    trackList.setSelected(track);
    juce::MessageManager::callAsync([this]() {
        trackListController.repaint();
        mixerController.repaint();
    });
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
        desktopComponent.resized();
    });
}

void DesktopController::verticalScaleDecreased() {
    project.decrementVerticalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleIncreased() {
    project.incrementHorizontalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        desktopComponent.resized();
    });
}

void DesktopController::horizontalScaleDecreased() {
    project.decrementHorizontalScale();
    juce::MessageManager::callAsync([this]() {
        trackListController.update();
        desktopComponent.resized();
    });
}
