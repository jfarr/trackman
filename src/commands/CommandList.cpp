#include "CommandList.h"

CommandList::CommandList() {}

CommandList::~CommandList() {
    for (Command *command : commands) {
        std::unique_ptr<Command> toDelete;
        toDelete.reset(command);
    }
}

void CommandList::pushCommand(Command *command) {
    commands.push_back(command);
    command->execute();
}

Command *CommandList::popCommand() {
    if (commands.size() == 0) {
        return nullptr;
    }
    Command *command = commands.back();
    commands.pop_back();
    return command;
}

juce::String CommandList::getLastCommandName() const {
    if (commands.size() == 0) {
        return "";
    }
    return commands.back()->getName();
}
