#pragma once

#include "Command.h"
#include "controls/desktop/TrackListController.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(TrackListController &controller);
    ~AddTrackCommand() override;

    void execute() override { track = controller.addNewTrack(); }
    void undo() override { controller.removeTrack(track); }

  private:
    TrackListController &controller;
    Track *track;
};

class ChangeMasterVolumeCommand : public Command {
  public:
    ChangeMasterVolumeCommand(MixerComponent &mixer, float previousLevel);
    ~ChangeMasterVolumeCommand() override;

    void execute() override {}
    void undo() override { mixer.setLevel(previousLevel); }

  private:
    MixerComponent &mixer;
    float previousLevel;
};
