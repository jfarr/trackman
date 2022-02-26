#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent : public juce::AudioAppComponent, public juce::ChangeListener, public juce::Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    // AudioAppComponent
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    // Component
    void paint (juce::Graphics& g) override;
    void resized() override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==============================================================================
    // Timer
    void MainComponent::timerCallback() override;

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };

    TransportState state;
    juce::AudioTransportSource transportSource;

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;

    void changeState(TransportState newState);
    void updateLoopState(bool shouldLoop);

    //==============================================================================

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void loopButtonChanged();

    std::unique_ptr<juce::FileChooser> chooser;

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
