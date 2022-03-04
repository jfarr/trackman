#include "CommandList.h"

CommandList::CommandList() {}

CommandList::~CommandList() {}

void CommandList::pushCommand(Command *command) {
    std::unique_ptr<Command> p(command);
    commands.push_back(std::move(p));
    command->execute();
}

void CommandList::undoLast() {
    if (commands.size() == 0) {
        return;
    }
    commands.back()->undo();
    commands.pop_back();
}

juce::String CommandList::getLastCommandName() const {
    if (commands.size() == 0) {
        return "";
    }
    return commands.back()->getName();
}
