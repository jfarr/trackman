#pragma once

#include <JuceHeader.h>

#include "TrackRegion.h"
#include "audio/SynthAudioSource.h"

using namespace juce;

namespace trackman {

class NoteRoll : public PositionableAudioSource, public TrackRegion {
  public:
    NoteRoll(const MidiMessageSequence &midiMessages, double startPosInSeconds, double endPosInSeconds);
    ~NoteRoll() = default;

    const MidiMessageSequence &getMidiMessages() const { return midiMessages; }
    double getStartPosInSeconds() const { return startPosInSeconds; }
    double getEndPosInSeconds() const { return endPosInSeconds; }
    double getLengthInSeconds() const { return lengthInSeconds; }
    bool isSelected() const { return selected; }
    bool isDeleted() const { return deleted; }

    void setSelected(bool newSelected) { selected = newSelected; }
    void setDeleted(bool newDeleted) { deleted = newDeleted; }

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
    MidiMessageSequence midiMessages;
    double startPosInSeconds;
    double endPosInSeconds;
    double lengthInSeconds;
    bool selected = false;
    bool deleted = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoteRoll)
};

} // namespace trackman