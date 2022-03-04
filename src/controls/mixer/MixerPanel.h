#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "controls/common/TransportControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class MixerPanel : public juce::AudioAppComponent {
  public:
    MixerPanel(TrackList &trackList, Mixer &mixer);
    ~MixerPanel();

    void clear();
    void addTrack(TrackControl *trackControl);

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

  private:
    TransportControl transportControl;
    MasterTrackControl masterTrackControl;
    std::list<std::unique_ptr<TrackControl>> tracks;

    TrackList &trackList;
    Mixer &mixer;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
