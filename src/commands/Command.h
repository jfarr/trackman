#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class Command {
  public:
    Command(String name) : name(name) {}
    virtual ~Command() {}

    String getName() { return name; }

    virtual void execute() {}
    virtual void undo() = 0;

  private:
    String name;
};

} // namespace trackman
