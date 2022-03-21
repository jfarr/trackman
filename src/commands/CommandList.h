#pragma once

#include <JuceHeader.h>

#include "Command.h"

using namespace std;
using namespace juce;

namespace trackman {

class CommandList {
  public:
    CommandList();
    ~CommandList();

    void pushCommand(Command *command);
    void undoLast();
    void clear() { commands.clear(); }
    bool isEmpty() const { return commands.size() == 0; }
    String getLastCommandName() const;
    Command *peek() const;

  private:
    list<unique_ptr<Command>> commands;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandList)
};

} // namespace trackman
