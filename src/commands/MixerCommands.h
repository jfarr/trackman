#pragma once

#include <JuceHeader.h>

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

class ToggleMasterMuteCommand : public Command {
  public:
    ToggleMasterMuteCommand(MixerController &mixerController)
        : Command(juce::String(mixerController.getMixer().isMasterMuted() ? "mute" : "unmute") + " master"), mixerController(mixerController) {}
    ~ToggleMasterMuteCommand() {}

    void undo() override { mixerController.toggleMasterMute(); }

  private:
    MixerController &mixerController;
};

class ChangeTrackVolumeCommand : public Command {
  public:
    ChangeTrackVolumeCommand(MixerController &mixerController, Track &track, float previousLevel)
        : Command("change track " + juce::String(track.getNumber()) + " volume"), mixerController(mixerController), track(track),
          previousLevel(previousLevel) {}
    ~ChangeTrackVolumeCommand() override {}

    void undo() override { mixerController.setLevel(track, previousLevel); }

  private:
    MixerController &mixerController;
    Track &track;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeTrackVolumeCommand)
};

class ToggleMuteCommand : public Command {
  public:
    ToggleMuteCommand(MixerController &mixerController, Track &track)
        : Command(juce::String(track.isMuted() ? "mute" : "unmute") + " Track " + juce::String(track.getNumber())), mixerController(mixerController), track(track) {
    }
    ~ToggleMuteCommand() {}

    void undo() override { mixerController.toggleMute(track); }

  private:
    MixerController &mixerController;
    Track &track;
};

class ToggleSoloCommand : public Command {
  public:
    ToggleSoloCommand(MixerController &mixerController, Track &track)
        : Command(juce::String(track.isSoloed() ? "solo" : "unsolo") + " Track " + juce::String(track.getNumber())), mixerController(mixerController), track(track) {
    }
    ~ToggleSoloCommand() {}

    void undo() override { mixerController.toggleSolo(track); }

  private:
    MixerController &mixerController;
    Track &track;
};
