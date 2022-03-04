#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "TrackController.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class MixerController : public TrackSourceListener,
                        public TransportControlListener,
                        public MasterTrackListener,
                        public TrackListListener {
  public:
    MixerController(TrackList &trackList, juce::AudioFormatManager &formatManager);
    ~MixerController();

    MixerPanel &getMixerPanel() { return mixerPanel; }

    void update();

    void setMasterLevel(float newLevel);

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
    // TrackListListener
    void selectionChanged(Track &track, juce::Component *source) override;

  private:
    TrackList &trackList;
    Mixer mixer;
    MixerPanel mixerPanel;
    std::list<std::unique_ptr<TrackController>> tracks;
    juce::AudioFormatManager &formatManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
