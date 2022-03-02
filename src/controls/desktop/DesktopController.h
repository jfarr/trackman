#pragma once

#include "TrackListController.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopController : public MasterTrackListener {
  public:
    DesktopController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel);
    ~DesktopController() {}

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }

    void addNewTrack();

    //==============================================================================
    // MasterTrackListener
    void levelChangeFinalized(float previousLevel) override;

  private:
    CommandList commandList;
    TrackListController trackListController;
};
