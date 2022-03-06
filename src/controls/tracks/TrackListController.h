#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "TrackListPanel.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/desktop/TrackSourceListener.h"
#include "controls/tracks/SampleListener.h"
#include "model/TrackList.h"

class TrackListController : public TrackListListener, SampleListener {
  public:
    TrackListController(TrackList &trackList, juce::AudioTransportSource &transport,
        juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    ~TrackListController() {}

    TrackListPanel &getTrackListPanel() { return trackListPanel; }
    juce::Component &getViewport() { return trackListViewport; }

    Sample *addSample(Track &track, juce::File file, int pos);
    void deleteSample(Track &track, Sample *sample);

    void update();
    void repaint();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped(const juce::StringArray &files, int x, int y);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);
    void addListener(SampleListener *listener);
    void removeListener(SampleListener *listener);
    void setListener(class TrackSourceListener *newListener) { listener = newListener; }

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track) override;

    //==============================================================================
    // SampleListener
    void sampleSelected(Sample &sample) override;
    void sampleMoved(Sample &sample, juce::Point<int> pos) override;
    void dragEnded() override;

  private:
    TrackList &trackList;
    juce::AudioTransportSource &transport;
    TrackListPanel trackListPanel;
    juce::Viewport trackListViewport;
    float scale = 75;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
    std::list<TrackListListener *> trackListListeners;
    std::list<SampleListener *> sampleListeners;
    TrackSourceListener *listener = nullptr;
    juce::AudioDeviceManager &deviceManager;
    juce::AudioFormatManager &formatManager;

    void notifySelectionChanged(Track &track);
    void notifySampleAdded(Track &track, juce::File file, int pos);

    void updateLane(Track &track);
    TrackLaneController *getLane(Track &track);
    void updateLanes();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListController)
};
