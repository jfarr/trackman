#include "CommandList.h"

namespace trackman {

CommandList::CommandList() {}

CommandList::~CommandList() {}

void CommandList::pushCommand(Command *command) {
    unique_ptr<Command> p(command);
    commands.push_back(move(p));
    command->execute();
}

void CommandList::undoLast() {
    if (commands.size() == 0) {
        return;
    }
    commands.back()->undo();
    commands.pop_back();
}

String CommandList::getLastCommandName() const {
    if (commands.size() == 0) {
        return "";
    }
    return commands.back()->getName();
}

Command *CommandList::peek() const {
    if (commands.size() == 0) {
        return nullptr;
    }
    return commands.back().get();
}

} // namespace trackman
