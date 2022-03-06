#pragma once

#include <JuceHeader.h>

#include "TrackLaneControl.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Track.h"

class TrackLaneController : public juce::MouseListener, public TrackListListener, SampleListener {
  public:
    TrackLaneController(Track &track, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager);
    ~TrackLaneController() {}

    Track &getTrack() { return track; }
    TrackLaneControl &getTrackLaneControl() { return trackLaneControl; }

    void update();
    void repaint();

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);
    void addListener(SampleListener *listener);
    void removeListener(SampleListener *listener);

    //==============================================================================
    // MouseListener
    void mouseDown(const juce::MouseEvent &event) override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track) override;

    //==============================================================================
    // SampleListener
    void sampleSelected(Sample &sample) override;

  private:
    Track &track;
    juce::AudioTransportSource &transport;
    TrackLaneControl trackLaneControl;
    std::list<std::unique_ptr<SampleThumbnail>> thumbnails;
    std::list<TrackListListener *> trackListListeners;
    std::list<SampleListener *> sampleListeners;
    juce::AudioFormatManager &formatManager;

    void notifySelectionChanged();
    void notifySampleSelected(Sample &sample);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackLaneController)
};
