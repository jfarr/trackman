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
    ToggleMasterMuteCommand(MixerController &controller)
        : Command(juce::String(controller.isMasterMuted() ? "unmute" : "mute") + " master"),
          controller(controller), muted(controller.isMasterMuted()) {}
    ~ToggleMasterMuteCommand() {}

    void execute() override { controller.setMasterMute(!muted); }
    void undo() override { controller.setMasterMute(muted); }

  private:
    MixerController &controller;
    bool muted;
};

class ChangeTrackVolumeCommand : public Command {
  public:
    ChangeTrackVolumeCommand(MixerController &mixerController, Track &track, float previousLevel)
        : Command("change track " + juce::String(track.getNumber()) + " volume"), mixerController(mixerController),
          track(track), previousLevel(previousLevel) {}
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
    ToggleMuteCommand(MixerController &controller, Track &track)
        : Command(juce::String(track.isMuted() ? "unmute" : "mute") + " Track " + juce::String(track.getNumber())),
          controller(controller), track(track), muted(track.isMuted()) {}
    ~ToggleMuteCommand() {}

    void execute() override { controller.setMute(track, !muted); }
    void undo() override { controller.setMute(track, muted); }

  private:
    MixerController &controller;
    Track &track;
    bool muted;
};

class ToggleSoloCommand : public Command {
  public:
    ToggleSoloCommand(MixerController &controller, Track &track)
        : Command(juce::String(track.isSoloed() ? "unsolo" : "solo") + " Track " + juce::String(track.getNumber())),
          controller(controller), track(track), soloed(track.isSoloed()) {}
    ~ToggleSoloCommand() {}

    void execute() override { controller.setSolo(track, !soloed); }
    void undo() override { controller.setSolo(track, soloed); }

  private:
    MixerController &controller;
    Track &track;
    bool soloed;
};
