#pragma once

#include "Command.h"
#include "JuceHeader.h"

class CommandList {
  public:
    CommandList();
    ~CommandList();

    void pushCommand(Command *command);
    void undoLast();
    bool isEmpty() const { return commands.size() == 0; }
    juce::String getLastCommandName() const;
    Command *peek() const;

  private:
    std::list<std::unique_ptr<Command>> commands;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandList)
};
