#pragma once

#include <JuceHeader.h>

#include "Command.h"
#include "ui/desktop/DesktopController.h"

class ChangeMasterVolumeCommand : public Command {
  public:
    ChangeMasterVolumeCommand(trackman::MixerController &mixerController, float previousLevel)
        : Command("Change Master Volume"), mixerController(mixerController), previousLevel(previousLevel) {}
    ~ChangeMasterVolumeCommand() override = default;

    void undo() override { mixerController.setMasterLevel(previousLevel); }

  private:
    trackman::MixerController &mixerController;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeMasterVolumeCommand)
};

class ToggleMasterMuteCommand : public Command {
  public:
    explicit ToggleMasterMuteCommand(trackman::MixerController &controller)
        : Command(juce::String(controller.isMasterMuted() ? "Unmute" : "Mute") + " Master"), controller(controller),
          muted(controller.isMasterMuted()) {}
    ~ToggleMasterMuteCommand() override = default;

    void execute() override { controller.setMasterMute(!muted); }
    void undo() override { controller.setMasterMute(muted); }

  private:
    trackman::MixerController &controller;
    bool muted;
};

class ChangeTrackVolumeCommand : public Command {
  public:
    ChangeTrackVolumeCommand(trackman::MixerController &mixerController, Track &track, float previousLevel)
        : Command("Change Track " + juce::String(track.getTrackNumber()) + " Volume"), mixerController(mixerController),
          track(track), previousLevel(previousLevel) {}
    ~ChangeTrackVolumeCommand() override = default;

    void undo() override { mixerController.setLevel(track, previousLevel); }

  private:
    trackman::MixerController &mixerController;
    Track &track;
    float previousLevel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChangeTrackVolumeCommand)
};

class ToggleMuteCommand : public Command {
  public:
    ToggleMuteCommand(trackman::MixerController &controller, Track &track)
        : Command(juce::String(track.isMuted() ? "Unmute" : "Mute") + " Track " + juce::String(track.getTrackNumber())),
          controller(controller), track(track), muted(track.isMuted()) {}
    ~ToggleMuteCommand() override = default;

    void execute() override { controller.setMute(track, !muted); }
    void undo() override { controller.setMute(track, muted); }

  private:
    trackman::MixerController &controller;
    Track &track;
    bool muted;
};

class ToggleSoloCommand : public Command {
  public:
    ToggleSoloCommand(trackman::MixerController &controller, Track &track)
        : Command(juce::String(track.isSoloed() ? "Unsolo" : "Solo") + " Track " + juce::String(track.getTrackNumber())),
          controller(controller), track(track), soloed(track.isSoloed()) {}
    ~ToggleSoloCommand() override = default;

    void execute() override { controller.setSolo(track, !soloed); }
    void undo() override { controller.setSolo(track, soloed); }

  private:
    trackman::MixerController &controller;
    Track &track;
    bool soloed;
};
