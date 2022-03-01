#pragma once

#include "TrackController.h"
#include "controls/mixer/MixerComponent.h"
#include "controls/tracks/TrackListPanel.h"
#include "model/Track.h"
#include "model/TrackList.h"

class TrackListListener {
  public:
    virtual void trackAdded(Track &track) = 0;
};

class TrackListController : public MixerComponentListener {
  public:
    TrackListController(juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel);
    ~TrackListController();

    void addNewTrack();

    void mixerResized(juce::Rectangle<int> area);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

  private:
    juce::AudioFormatManager &formatManager;
    TrackList trackList;
    MixerComponent &mixer;
    TrackListPanel &trackListPanel;
    std::list<TrackController *> tracks;
    std::list<TrackListListener *> listeners;

    void eachTrack(std::function<void(TrackController &)> f);
    void notifyTrackAdded(Track &track);
};
