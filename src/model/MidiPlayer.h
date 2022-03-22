#pragma once

#include <JuceHeader.h>

#include "Project.h"
#include "audio/SynthAudioSource.h"

using namespace juce;

namespace trackman {

class MidiPlayer : public PositionableAudioSource {
  public:
    MidiPlayer(Project &project, SynthAudioSource &synthAudioSource)
        : project(project), synthAudioSource(synthAudioSource) {}
    ~MidiPlayer() = default;

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override { synthAudioSource.releaseResources(); }

    //==============================================================================
    // PositionableAudioSource
    void setNextReadPosition(int64 newPosition) override { synthAudioSource.setNextReadPosition(newPosition); }
    int64 getNextReadPosition() const override { return synthAudioSource.getNextReadPosition(); }
    int64 getTotalLength() const override { return synthAudioSource.getTotalLength(); }
    bool isLooping() const override { return synthAudioSource.isLooping(); }
    void setLooping(bool shouldLoop) override { synthAudioSource.setLooping(shouldLoop); }

  private:
    Project &project;
    SynthAudioSource &synthAudioSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiPlayer)
};

} // namespace trackman
