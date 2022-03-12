#pragma once

#include <JuceHeader.h>

class TransportControlListener {
  public:
    virtual void loopingChanged(bool shouldLoop) = 0;
};

class TransportControl : public juce::Component, public juce::ChangeListener, public juce::Timer {
  public:
    //==============================================================================
    TransportControl(juce::AudioTransportSource &transportSource, bool enabled = true);
    ~TransportControl() override;

    void setEnabled(bool enabled);
    void addListener(TransportControlListener *listener);
    void removeListener(TransportControlListener *listener);

    //==============================================================================
    // Component
    void paint(juce::Graphics &g) override;
    void resized() override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;

    //==============================================================================
    // Timer
    void timerCallback() override;

  private:
    enum class TransportState { Stopped, Starting, Playing, Pausing, Paused, Stopping };
    TransportState state = TransportState::Stopped;
    juce::AudioTransportSource &transportSource;
    bool enabled;
    std::list<TransportControlListener *> listeners;

    void createControls();
    void changeState(TransportState newState);
    juce::String getStateLabel();
    void notifyLoopingChanged(bool shouldLoop);

    //==============================================================================

    void startButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void loopButtonClicked();

    const float buttonHeight = 210;
    const float buttonWidth = 500;

    juce::ImageButton startButton;
    juce::ImageButton playButton;
    juce::ImageButton stopButton;
    juce::ImageButton pauseButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;

    juce::Image startButtonImage;
    juce::Image playButtonOffImage;
    juce::Image playButtonOnImage;
    juce::Image stopButtonImage;
    juce::Image pauseButtonOffImage;
    juce::Image pauseButtonOnImage;

    void drawStartButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawPlayButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawStopButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawPauseButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    static void setButtonImage(juce::ImageButton &button, juce::Image &image);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControl)
};
