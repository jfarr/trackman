#include "DesktopController.h"
#include "commands/TrackCommands.h"

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
