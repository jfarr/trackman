#pragma once

#include "TrackControllerListener.h"
#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerPanel.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopController : public MixerPanelListener, public MasterTrackListener, public TrackControllerListener {
  public:
    DesktopController(juce::AudioFormatManager &formatManager, MixerPanel &mixerPanel, TrackListPanel &trackListPanel);
    ~DesktopController();

    MixerPanel &getMixer() { return mixerPanel; }

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }

    //    void addNewTrack();

    Track *addNewTrack();
    void removeTrack(Track *track);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MixerPanelListener
    void mixerResized(juce::Rectangle<int> area) override;

    //==============================================================================
    // MasterTrackListener
    void levelChangeFinalized(float previousLevel) override;

    //==============================================================================
    // TrackControllerListener
    void selectionChanged(TrackController *selected) override;

  private:
    CommandList commandList;
    TrackList trackList;
    std::list<TrackController *> tracks;
    std::list<TrackListListener *> listeners;

    juce::AudioFormatManager &formatManager;
    MixerPanel &mixerPanel;
    TrackListPanel &trackListPanel;

    void notifyTrackAdded(Track &track);
    TrackController *getController(const Track *track);
};
