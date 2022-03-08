#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "controls/desktop/DesktopController.h"
#include "controls/mixer/TrackController.h"
#include "model/Track.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(DesktopController &controller)
        : Command("add track"), controller(controller) {}
    ~AddTrackCommand() override {}

    void execute() override { track = controller.addTrack(); }
    void undo() override { controller.deleteTrack(track, true); }

  private:
    DesktopController &controller;
    Track *track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};

class DeleteTrackCommand : public Command {
  public:
    DeleteTrackCommand(DesktopController &controller, Track *track)
        : Command("delete " + track->getName()), controller(controller), track(track) {}
    ~DeleteTrackCommand() override {}

    void execute() override { controller.deleteTrack(track, false); }
    void undo() override { controller.undeleteTrack(track); }

  private:
    DesktopController &controller;
    Track *track;
};
