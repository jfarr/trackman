#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "ui/desktop/DesktopController.h"

using namespace juce;

namespace trackman {

class ChangeTempoCommand : public Command {
  public:
    ChangeTempoCommand(DesktopController &desktopController, float previousTempo, float newTempo)
        : Command("Change Tempo"), desktopController(desktopController), previousTempo(previousTempo),
          newTempo(newTempo) {}
    ~ChangeTempoCommand() override = default;

    void execute() override { desktopController.changeTempo(newTempo); }
    void undo() override { desktopController.changeTempo(previousTempo); }

  private:
    DesktopController &desktopController;
    float previousTempo;
    float newTempo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeTempoCommand)
};

} // namespace trackman
