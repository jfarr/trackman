#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "controls/common/TransportControl.h"
#include "model/Mixer.h"
#include "MixerPanel.h"

class MixerController : public TrackSourceListener, public TransportControlListener, public MasterTrackListener {
  public:
    MixerController(Mixer &mixer, MixerPanel &mixerPanel);
    ~MixerController();

    void setMasterLevel(float newLevel);

    //==============================================================================
    // TrackSourceListener
    void onSourceSet(std::shared_ptr<juce::PositionableAudioSource> newSource, std::shared_ptr<juce::PositionableAudioSource> prevSource,
        const bool deleteWhenRemoved, double sourceSampleRateToCorrectFor = 0.0, int maxNumChannels = 2) override;

    //==============================================================================
    // TransportControlListener
    void loopingChanged(bool shouldLoop) override;

    //==============================================================================
    // MasterTrackListener
    void masterLevelChanged(float level) override;
    void masterMuteToggled() override;

  private:
    Mixer &mixer;
    MixerPanel &mixerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerController)
};
