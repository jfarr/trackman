#pragma once

#include "TrackListController.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopController {
  public:
    DesktopController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel)
        : trackListController(formatManager, mixer, trackListPanel) {}
    ~DesktopController() {}

    void addNewTrack();

    void undoLast();
    bool canUndo() const;
    juce::String getLastCommandName() { return commandList.getLastCommandName(); }

  private:
    CommandList commandList;
    TrackListController trackListController;
};
