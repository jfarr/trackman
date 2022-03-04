#pragma once

#include <JuceHeader.h>

#include "MasterTrackControl.h"
#include "MixerPanelListener.h"
#include "TrackControl.h"
#include "controls/common/TransportControl.h"
#include "model/Mixer.h"

class MixerPanel : public juce::AudioAppComponent {
  public:
    MixerPanel(Mixer &mixer);
    ~MixerPanel();

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }

    void addTrack(TrackControl &trackControl);
    void removeTrack(TrackControl &trackControl);

    void addListener(MixerPanelListener *listener);
    void removeListener(MixerPanelListener *listener);

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
    Mixer &mixer;

    std::list<MixerPanelListener *> listeners;

    void createControls();
    void notifyResized(juce::Rectangle<int> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};