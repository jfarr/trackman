#pragma once

#include "TrackControllerListener.h"
#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerController.h"
#include "controls/mixer/MixerPanel.h"
#include "controls/tracks/TrackListPanel.h"
#include "model/TrackList.h"

class DesktopController : public MixerPanelListener,
                          public MasterTrackListener,
                          public TrackControlListener,
                          public TrackControllerListener {
  public:
    DesktopController(
        juce::AudioFormatManager &formatManager, Mixer &mixer, MixerPanel &mixerPanel, TrackListPanel &trackListPanel);
    ~DesktopController();

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }

    void addNewTrack();
    void deleteSelectedTrack();

    TrackController *addTrack(juce::String name);
    void addTrackController(TrackController *track);
    void removeTrackController(TrackController *track);

    //==============================================================================
    // MixerPanelListener
    void mixerResized(juce::Rectangle<int> area) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChangeFinalized(float previousLevel) override;

    //==============================================================================
    // TrackControlListener
    void levelChangeFinalized(TrackControl &trackControl, float previousLevel) override;

    //==============================================================================
    // TrackControllerListener
    void selectionChanged(TrackController *newSelected) override;

  private:
    CommandList commandList;
    TrackList trackList;
    std::list<TrackController *> tracks;
    TrackController *selected = nullptr;
    MixerController mixerController;

    juce::AudioFormatManager &formatManager;
    Mixer &mixer;
    MixerPanel &mixerPanel;
    TrackListPanel &trackListPanel;
};
