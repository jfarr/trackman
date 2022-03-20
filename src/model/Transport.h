#pragma once

#include <JuceHeader.h>

class Transport : public juce::AudioSource, private juce::Timer {
  public:
    Transport(juce::PositionableAudioSource &source);
    ~Transport() override = default;

    void setPosition(double newPosition) { transportSource.setPosition(newPosition); }
    double getCurrentPosition() const { return transportSource.getCurrentPosition(); }
    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }
    bool isPlaying() const noexcept { return transportSource.isPlaying(); }

    //==============================================================================
    // AudioSource
    void prepareToPlay(int blockSize, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;

  private:
    juce::AudioTransportSource transportSource;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Transport)
};
