#pragma once

#include <JuceHeader.h>

#include "TrackLaneController.h"
#include "TrackListPanel.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/tracks/SampleListener.h"
#include "model/TrackList.h"

class DesktopController;

class TrackListController : public TrackListListener, public SampleListener {
  public:
    TrackListController(DesktopController &desktop, TrackList &trackList, juce::AudioTransportSource &transport,
        juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager);
    ~TrackListController();

    TrackListPanel &getTrackListPanel() { return trackListPanel; }
    juce::Component &getViewport() { return trackListViewport; }

    Sample *addSample(Track &track, juce::File file, int pos);
    void moveSample(Sample &sample, Track &fromTrack, Track &toTrack, double pos);
    void deleteSample(Track &track, Sample *sample);
    void undeleteSample(Track &track, Sample *sample);
//    void moveSample(Sample &sample, double pos);
    void resizeSample(Sample &sample, double length);

    void update();
    void repaint();

    void fileDragEnter(const juce::StringArray &files, int x, int y);
    void fileDragMove(const juce::StringArray &files, int x, int y);
    void fileDragExit(const juce::StringArray &files);
    void filesDropped(const juce::StringArray &files, int x, int y);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

    //==============================================================================
    // SampleListener
    void sampleSelected(Track &track, Sample &sample) override;
    void sampleMoved(Track &track, Sample &sample, int x, int y) override;
    void sampleResized(Sample &sample, int width) override;
    void dragEnded() override;

  private:
    DesktopController &desktop;
    TrackList &trackList;
    juce::AudioTransportSource &transport;
    TrackListPanel trackListPanel;
    juce::Viewport trackListViewport;
    float scale = 75;
    std::list<std::unique_ptr<TrackLaneController>> lanes;
    Track *selected = nullptr;
    bool selectingSample = false;

    juce::AudioDeviceManager &deviceManager;
    juce::AudioFormatManager &formatManager;

    void updateMixerSource();
    void updateLane(Track &track);
    TrackLaneController *getLane(Track &track);
    void updateLanes();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrackListController)
};
