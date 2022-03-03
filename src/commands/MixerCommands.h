#pragma once

#include "Command.h"
#include "controls/desktop/DesktopController.h"

class ChangeMasterVolumeCommand : public Command {
  public:
    ChangeMasterVolumeCommand(MixerPanel &mixerPanel, float previousLevel)
        : Command("change master volume"), mixerPanel(mixerPanel), previousLevel(previousLevel) {}
    ~ChangeMasterVolumeCommand() override {}

    void undo() override { mixerPanel.setMasterLevel(previousLevel); }

  private:
    MixerPanel &mixerPanel;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeMasterVolumeCommand)
};

class ChangeTrackVolumeCommand : public Command {
  public:
    ChangeTrackVolumeCommand(TrackControl &trackControl, float previousLevel)
        : Command("change " + trackControl.getTrackName() + " volume"), trackControl(trackControl),
          previousLevel(previousLevel) {}
    ~ChangeTrackVolumeCommand() override {}

    void undo() override { trackControl.setLevel(previousLevel); }

  private:
    TrackControl &trackControl;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeTrackVolumeCommand)
};
