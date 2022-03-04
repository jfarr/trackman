#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"

class ChangeMasterVolumeCommand : public Command {
  public:
    ChangeMasterVolumeCommand(MixerController &mixerController, float previousLevel)
        : Command("change master volume"), mixerController(mixerController), previousLevel(previousLevel) {}
    ~ChangeMasterVolumeCommand() override {}

    void undo() override { mixerController.setMasterLevel(previousLevel); }

  private:
    MixerController &mixerController;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeMasterVolumeCommand)
};

class ChangeTrackVolumeCommand : public Command {
  public:
    ChangeTrackVolumeCommand(MixerController &mixerController, Track &track, float previousLevel)
        : Command("change " + track.getName() + " volume"), mixerController(mixerController), track(track),
          previousLevel(previousLevel) {}
    ~ChangeTrackVolumeCommand() override {}

    void undo() override { mixerController.setLevel(track, previousLevel); }

  private:
    MixerController &mixerController;
    Track &track;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeTrackVolumeCommand)
};
