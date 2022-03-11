#pragma once

#include "JuceHeader.h"

#include "audio/GainAudioSource.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/mixer/TrackControl.h"
#include "controls/tracks/TrackLaneControl.h"
#include "model/Track.h"

class MixerController;

class TrackController : public TrackControlListener, public juce::MouseListener {
  public:
    TrackController(MixerController &mixerController, Track &track, TrackControl &trackControl,
        juce::AudioFormatManager &formatManager);
    ~TrackController();

    Track &getTrack() const { return track; }

    void setLevel(float newLevel);
    void toggleMute(Track &track);
    void toggleSolo(Track &track);
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);
//    void addListener(TrackControlListener *listener);
//    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // TrackControlListener
    void nameChanged(Track &track, juce::String newName) override;
    void levelChanged(Track &track, float newLevel) override;
    void levelChangeFinalized(Track &track, float previousLevel) override;
    void muteToggled(Track &track) override;
    void soloToggled(Track &track) override;

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

  private:
    MixerController &mixerController;
    Track &track;
    TrackControl &trackControl;
    juce::AudioFormatManager &formatManager;

    std::list<TrackListListener *> trackListListeners;
//    std::list<TrackControlListener *> trackControlListeners;

    void notifySelectionChanged();
//    void notifyNameChanged(Track &track, juce::String newName);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackController)
};
