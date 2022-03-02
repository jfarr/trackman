#pragma once

#include "TrackListController.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopController : public MasterTrackListener {
  public:
    DesktopController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel);
    ~DesktopController() {}

    void addNewTrack();

    void undoLast();
    bool canUndo() const;
    juce::String getLastCommandName() { return commandList.getLastCommandName(); }

    //==============================================================================
    // MasterTrackListener
    void levelChangeFinalized(float previousLevel) override;

  private:
    CommandList commandList;
    TrackListController trackListController;
};
