#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackListCommands.h"
#include "common/listutil.h"

DesktopController::DesktopController(juce::DocumentWindow &mainWindow, juce::AudioFormatManager &formatManager)
    : mixerController(trackList, formatManager), trackListController(trackList),
      project(trackList, mixerController.getMixer()), mainWindow(mainWindow), applicationName(mainWindow.getName()),
      formatManager(formatManager) {
    mixerController.addListener((TrackListListener *)this);
    mixerController.addListener((MasterTrackListener *)this);
    mixerController.addListener((TrackControlListener *)this);
    trackListController.addListener(this);
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

void DesktopController::updateTitleBar() { mainWindow.setName(applicationName + (dirty ? " [modified]" : "")); }

void DesktopController::selectionChanged(Track &track) {
    trackList.setSelected(track);
    trackListController.repaint();
    mixerController.repaint();
}
