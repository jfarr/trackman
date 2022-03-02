#include "DesktopController.h"
#include "commands/TrackCommands.h"

DesktopController::DesktopController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel)
    : trackListController(formatManager, mixer, trackListPanel) {
    mixer.getMasterTrackControl().addListener(this);
}

void DesktopController::addNewTrack() {
    Command *command = new AddTrackCommand(trackListController);
    commandList.pushCommand(command);
    command->execute();
}

void DesktopController::undoLast() {
    Command *command = commandList.popCommand();
    if (command != nullptr) {
        command->undo();
        delete command;
    }
}

bool DesktopController::canUndo() const {
    return !commandList.isEmpty();
}

void DesktopController::levelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(trackListController.getMixer(), previousLevel);
    commandList.pushCommand(command);
}
