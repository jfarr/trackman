#pragma once

#include "TrackListListener.h"
#include "commands/CommandList.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"

class DesktopController : public MixerComponentListener, public MasterTrackListener, public TrackControllerListener {
  public:
    DesktopController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel);
    ~DesktopController();

    MixerComponent &getMixer() { return mixer; }

    bool canUndo() const;
    void undoLast();
    juce::String getLastCommandName() const { return commandList.getLastCommandName(); }

//    void addNewTrack();

    Track *addNewTrack();
    void removeTrack(Track *track);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    //==============================================================================
    // MixerComponentListener
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
    MixerComponent &mixer;
    TrackListPanel &trackListPanel;

    void notifyTrackAdded(Track &track);
    TrackController *getController(const Track *track);
};
