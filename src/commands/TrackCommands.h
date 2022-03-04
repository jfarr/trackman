#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"
#include "controls/mixer/TrackController.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(DesktopController &controller, juce::String name)
        : Command("add track"), controller(controller), name(name) {}
    ~AddTrackCommand() override {}

    void execute() override { track = controller.addTrack(name); }
    void undo() override { controller.removeTrackController(track); }

  private:
    DesktopController &controller;
    juce::String name;
    TrackController *track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};

class DeleteTrackCommand : public Command {
  public:
    DeleteTrackCommand(DesktopController &controller, TrackController *track)
        : Command("delete " + track->getTrack().getName()), controller(controller), track(track) {}
    ~DeleteTrackCommand() override {}

    void execute() override { controller.removeTrackController(track); }
    void undo() override { controller.addTrackController(track); }

  private:
    DesktopController &controller;
    TrackController *track;
};
