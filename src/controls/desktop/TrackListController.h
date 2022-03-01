#pragma once

#include "controls/tracks/TrackListPanel.h"
#include "controls/mixer/MixerComponent.h"
#include "model/Track.h"
#include "model/TrackList.h"

class TrackListListener {
  public:
    virtual void trackAdded(Track &track) = 0;
};

class TrackListController : public MixerComponentListener {
  public:
    TrackListController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel);
    ~TrackListController() = default;

    void addNewTrack();

    void mixerResized(juce::Rectangle<int> area);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

  private:
    juce::AudioFormatManager &formatManager;
    TrackList trackList;
    MixerComponent &mixer;
    TrackListPanel &trackListPanel;
    std::list<TrackListListener *> listeners;

    void notifyTrackAdded(Track &track);
};
