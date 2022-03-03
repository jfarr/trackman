#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(DesktopController &controller, juce::String name)
        : Command("add track"), controller(controller), name(name) {}
    ~AddTrackCommand() override {
        if (shouldDelete) {
            controller.deleteTrackController(track);
        }
    }

    void execute() override { track = controller.addTrack(name); }
    void undo() override {
        controller.removeTrackController(track);
        shouldDelete = true;
    }

  private:
    DesktopController &controller;
    juce::String name;
    TrackController *track;
    bool shouldDelete = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};

class DeleteTrackCommand : public Command {
  public:
    DeleteTrackCommand(DesktopController &controller, TrackController *track)
        : Command("delete " + track->getTrack().getName()), controller(controller), track(track) {}
    ~DeleteTrackCommand() override {
        if (shouldDelete) {
            controller.deleteTrackController(track);
        }
    }

    void execute() override {
        controller.removeTrackController(track);
        shouldDelete = false;
    }
    void undo() override {
        controller.addTrackController(track);
        std::cout << "OK\n";
    }

  private:
    DesktopController &controller;
    TrackController *track;
    bool shouldDelete = true;
};
