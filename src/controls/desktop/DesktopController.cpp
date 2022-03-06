#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackListCommands.h"
#include "common/listutil.h"

DesktopController::DesktopController(
    juce::DocumentWindow &mainWindow, juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager)
    : mixer(deviceManager.getAudioDeviceSetup().sampleRate), mixerController(trackList, mixer, formatManager),
      trackListController(trackList, mixerController.getMixer().getTransportSource(), deviceManager, formatManager),
      project(trackList, mixer), mainWindow(mainWindow), applicationName(mainWindow.getName()),
      deviceManager(deviceManager), formatManager(formatManager) {
    mixerController.addListener((TrackListListener *)this);
    mixerController.addListener((MasterTrackListener *)this);
    mixerController.addListener((TrackControlListener *)this);
    trackListController.addListener(this);
    trackListController.setListener(&mixerController);
}

DesktopController::~DesktopController() {}

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

void DesktopController::levelChangeFinalized(Track &track, float previousLevel) {
    Command *command = new ChangeTrackVolumeCommand(mixerController, track, previousLevel);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::muteToggled(Track &track) {
    Command *command = new ToggleMuteCommand(mixerController, track);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::resize() { getTrackListController().getTrackListPanel().resize(); }

void DesktopController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Command *command = new AddTrackCommand(*this, name);
    commandList.pushCommand(command);
    dirty = true;
    updateTitleBar();
}

void DesktopController::deleteSelectedTrack() {
    trackList.eachTrack([this](Track &track) {
        if (track.isSelected()) {
            Command *command = new DeleteTrackCommand(*this, &track);
            commandList.pushCommand(command);
            dirty = true;
            updateTitleBar();
        }
    });
}

Track *DesktopController::addTrack(juce::String name) {
    auto track = trackList.addTrack(name);
    trackListController.update();
    mixerController.update();
    return track;
}

void DesktopController::deleteTrack(Track *track, bool purge) {
    track->setDeleted(true);
    trackListController.update();
    mixerController.update();
    if (purge) {
        trackList.removeTrack(track);
    }
}

void DesktopController::undeleteTrack(Track *track) {
    track->setDeleted(false);
    trackListController.update();
    mixerController.update();
}

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

void DesktopController::updateTitleBar() {
    mainWindow.setName(
        (projectFile != juce::File{} ? projectFile.getFileName() + (dirty ? " [modified]" : "") + " - " : "") +
        applicationName);
}

void DesktopController::selectionChanged(Track &track) {
    trackList.setSelected(track);
    trackListController.repaint();
    mixerController.repaint();
}

void DesktopController::fileDragEnter(const juce::StringArray &files, int x, int y) {
    //    std::cout << "drag enter: " << files.joinIntoString(",").toStdString() << "\n";
    trackListController.fileDragEnter(files, x, y);
}

void DesktopController::fileDragMove(const juce::StringArray &files, int x, int y) {
    //    std::cout << "drag enter: " << files.joinIntoString(",").toStdString() << "\n";
    trackListController.fileDragMove(files, x, y);
}

void DesktopController::fileDragExit(const juce::StringArray &files) {
    //    std::cout << "drag enter: " << files.joinIntoString(",").toStdString() << "\n";
    trackListController.fileDragExit(files);
}

void DesktopController::filesDropped(const juce::StringArray &files, int x, int y) {
    //    std::cout << "drag enter: " << files.joinIntoString(",").toStdString() << "\n";
    trackListController.filesDropped(files, x, y);
}
