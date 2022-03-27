#pragma once

#include <JuceHeader.h>

#include "MidiHandler.h"

using namespace juce;

namespace trackman {

class InstrumentPlayer : public MidiHandler, public AudioSource {
  public:
    InstrumentPlayer(Project &project);
    ~InstrumentPlayer() override = default;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

  private:
    MidiMessageCollector midiCollector;
    double startTime;

    void midiMessageReceived(const MidiMessage &message, const double time);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InstrumentPlayer)
};

} // namespace trackman
