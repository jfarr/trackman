#pragma once

#include <JuceHeader.h>

class TransportControl : public juce::Component, public juce::Timer
{
public:
    //==============================================================================
    TransportControl(juce::AudioTransportSource *transportSource);
    ~TransportControl() override;

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // Timer
    void timerCallback() override;

private:
    enum class TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };
    TransportState state = TransportState::Stopped;
    juce::AudioTransportSource *transportSource;

    void createControls();
    void changeState(TransportState newState);
    void updateLoopState(bool shouldLoop);

    //==============================================================================

    void playButtonClicked();
    void stopButtonClicked();
    void loopButtonChanged();

    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;
};
