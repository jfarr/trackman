#pragma once

#include "JuceHeader.h"

class Command {
  public:
    Command(juce::String name) : name(name) {}
    virtual ~Command() {}

    juce::String getName() { return name; }

    virtual void execute() = 0;
    virtual void undo() = 0;

  private:
    juce::String name;
};
