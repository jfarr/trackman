#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "TrackControl.h"
#include "TrackControlListener.h"
#include "TrackController.h"
#include "controls/common/TransportControl.h"
//#include "controls/desktop/TrackControllerListener.h"
#include "controls/desktop/TrackListListener.h"
#include "controls/desktop/TrackSourceListener.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class MixerController : public TrackListListener,
                        public TrackSourceListener,
                        public TransportControlListener,
                        public MasterTrackListener,
                        public TrackControlListener //,
                        //public TrackControllerListener
{
  public:
    MixerController(TrackList &trackList, juce::AudioFormatManager &formatManager);
    ~MixerController();

    MixerPanel &getMixerPanel() { return mixerPanel; }

    void update();
    void repaint();

    void setMasterLevel(float newLevel);
    void setLevel(Track &track, float newLevel);

    void addListener(TrackListListener *listener);
    void removeListener(TrackListListener *listener);

    void addListener(TrackControlListener *listener);
    void removeListener(TrackControlListener *listener);

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track &track, juce::Component *source) override;

    //==============================================================================
    // TrackSourceListener
    void onSourceSet(std::shared_ptr<juce::PositionableAudioSource> newSource,
        std::shared_ptr<juce::PositionableAudioSource> prevSource, const bool deleteWhenRemoved,
        double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;
    void masterMuteToggled() override;

    //==============================================================================
    // TrackControlListener
    void levelChangeFinalized(Track &track, float previousLevel) override;

    //==============================================================================
    // TrackControllerListener
//    void selectionChanged(TrackController *newSelected) override;

  private:
    TrackList &trackList;
    Mixer mixer;
    MixerPanel mixerPanel;
    std::list<std::unique_ptr<TrackController>> tracks;
    juce::AudioFormatManager &formatManager;
    std::list<TrackListListener *> trackListListeners;
    std::list<TrackControlListener *> trackControlListeners;

    void notifySelectionChanged(Track &track, juce::Component *source);
    void notifyLevelChangeFinalized(Track &track, float previousLevel);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
