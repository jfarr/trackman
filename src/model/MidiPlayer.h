#pragma once

#include <JuceHeader.h>

#include "audio/SynthAudioSource.h"
#include "NoteRoll.h"
#include "TimeRange.h"
#include "Timeline.h"

using namespace juce;

namespace trackman {

class Project;
class Track;

class MidiPlayer : public PositionableAudioSource {
  public:
    MidiPlayer(Track &track);
    ~MidiPlayer() override = default;

    Synthesiser &getSynth() { return synth; }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override;
    int64 getNextReadPosition() const override;
    int64 getTotalLength() const override;
    bool isLooping() const override;
    void setLooping(bool shouldLoop) override;

  private:
    Track &track;
    Synthesiser synth;
    list<shared_ptr<NoteRoll>> &noteRolls;
    double currentSampleRate = 0;
    int64 currentPosition = 0;
    bool looping = false;
    AudioBuffer<float> tempBuffer;

    Timeline<NoteRoll *> getCurrentTimeline();
    double getTimeAtPosition(int64 position) { return position / currentSampleRate; }
    void processNextMidiBuffer(
        MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) const;

    CriticalSection mutex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};

} // namespace trackman
