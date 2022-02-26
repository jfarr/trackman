#pragma once

#include <JuceHeader.h>

class TransportControlListener
{
public:
    virtual void updateLoopState(bool shouldLoop) = 0;
};


class TransportControl : public juce::Component, public juce::ChangeListener, public juce::Timer
{
public:
    //==============================================================================
    TransportControl(juce::AudioTransportSource *transportSource, bool enabled = true);
    ~TransportControl() override;

    void setEnabled(bool enabled);
    void AddListener(TransportControlListener* listener);
    void RemoveListener(TransportControlListener* listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

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
    bool enabled;
    std::list<TransportControlListener*> listeners;

    void createControls();
    void changeState(TransportState newState);
    juce::String getStateLabel();
    void updateLoopState(bool shouldLoop);

    //==============================================================================

    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void startButtonClicked();
    void loopButtonChanged();

    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton pauseButton;
    juce::TextButton startButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;
};
