#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"

class AddTrackCommand : public Command {
  public:
    AddTrackCommand(DesktopController &controller, Track *track);
    ~AddTrackCommand() override;

    void execute() override {}
    void undo() override { controller.removeTrack(track); }

  private:
    DesktopController &controller;
    Track *track;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AddTrackCommand)
};

class ChangeMasterVolumeCommand : public Command {
  public:
    ChangeMasterVolumeCommand(MixerPanel &mixer, float previousLevel);
    ~ChangeMasterVolumeCommand() override;

    void execute() override {}
    void undo() override { mixer.setLevel(previousLevel); }

  private:
    MixerPanel &mixer;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeMasterVolumeCommand)
};
