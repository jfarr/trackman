#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanel.h"
#include "controls/common/TransportControl.h"
#include "controls/desktop/TrackListListener.h"
#include "model/Mixer.h"

class MixerController : public TrackSourceListener,
                        public TransportControlListener,
                        public MasterTrackListener,
                        public TrackListListener {
  public:
    MixerController(Mixer &mixer, MixerPanel &mixerPanel);
    ~MixerController();

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
    Mixer &mixer;
    MixerPanel &mixerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
