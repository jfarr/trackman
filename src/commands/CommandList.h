#pragma once

#include "Command.h"
#include "JuceHeader.h"

class CommandList {
  public:
    CommandList();
    ~CommandList();

    void pushCommand(Command *command);
    Command *popCommand();
    bool isEmpty() const { return commands.size() == 0; }
    juce::String getLastCommandName() const;

  private:
    std::list<Command *> commands;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CommandList)
};
