#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(DesktopController &controller, Track *track)
        : Command("add track"), controller(controller), track(track) {}
    ~AddTrackCommand() override {}

    void undo() override { controller.removeTrack(track); }

  private:
    DesktopController &controller;
    Track *track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};
