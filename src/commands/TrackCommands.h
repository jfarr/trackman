#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "model/Track.h"

using namespace juce;

namespace trackman {

class RenameTrackCommand : public Command {
  public:
    RenameTrackCommand(DesktopController &desktopController, Track &track, String newName)
        : Command("Rename Track"), desktopController(desktopController), track(track), newName(move(newName)),
          prevName(track.getName()) {}
    ~RenameTrackCommand() override = default;

    void execute() override { desktopController.renameTrack(track, newName); }
    void undo() override { desktopController.renameTrack(track, prevName); }

  private:
    DesktopController &desktopController;
    Track &track;
    String newName;
    String prevName;
};

} // namespace trackman
