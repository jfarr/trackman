#pragma once

#include <JuceHeader.h>
#include <ff_meters.h>

#include "MasterTrackControl.h"
#include "TrackControl.h"
#include "controls/common/TransportControl.h"
#include "model/Mixer.h"
#include "model/TrackList.h"

class MixerPanel : public juce::Component {
  public:
    MixerPanel(TrackList &trackList, Mixer &mixer, foleys::LevelMeterSource &meterSource);
    ~MixerPanel();

    int getPreferredHeight() const { return std::max(getHeight(), 280); }

    void clear();
    void update();
    void addTrack(TrackControl *trackControl);

    TransportControl &getTransportControl() { return transportControl; }
    MasterTrackControl &getMasterTrackControl() { return masterTrackControl; }
    foleys::LevelMeter &getLevelMeter() { return levelMeter; }
    void eachTrack(std::function<void(TrackControl &)> f);

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
    foleys::LevelMeter levelMeter;

    void createControls();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MixerPanel)
};
