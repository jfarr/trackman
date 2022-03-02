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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeMasterVolumeCommand)
};
