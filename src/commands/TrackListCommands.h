#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "model/Track.h"
#include "ui/desktop/DesktopController.h"
#include "ui/mixer/TrackController.h"

class AddTrackCommand : public Command {
  public:
    explicit AddTrackCommand(trackman::DesktopController &controller) : Command("Add Track"), controller(controller) {}
    ~AddTrackCommand() override = default;

    void execute() override { track = controller.addTrack(); }
    void undo() override { controller.deleteTrack(track, true); }

  private:
    trackman::DesktopController &controller;
    Track *track = nullptr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};

class DeleteTrackCommand : public Command {
  public:
    DeleteTrackCommand(trackman::DesktopController &controller, Track *track)
        : Command("Delete Track " + juce::String(track->getTrackNumber())), controller(controller), track(track) {}
    ~DeleteTrackCommand() override {}

    void execute() override { controller.deleteTrack(track, false); }
    void undo() override { controller.undeleteTrack(track); }

  private:
    trackman::DesktopController &controller;
    Track *track;
};
