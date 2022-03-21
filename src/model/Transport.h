#pragma once

#include <JuceHeader.h>

using namespace juce;

namespace trackman {

class Transport : public AudioSource, private Timer {
  public:
    Transport(PositionableAudioSource &source);
    ~Transport() override = default;

    AudioTransportSource &getTransportSource() { return transportSource; }

    void setPosition(double newPosition) { transportSource.setPosition(newPosition); }
    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }
    bool isPlaying() const noexcept { return transportSource.isPlaying(); }
    void setLooping(bool shouldLoop) { transportSource.setLooping(shouldLoop); }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;

  private:
    AudioTransportSource transportSource;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transport)
};

} // namespace trackman
