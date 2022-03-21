#pragma once

#include "audio/MidiRecorder.h"
#include "model/TrackList.h"
#include "ui/desktop/TrackListListener.h"
#include <JuceHeader.h>

namespace trackman {

class TransportControl : public juce::Component,
                         public juce::ChangeListener,
                         public juce::Timer,
                         public TrackListListener {
  public:
    //==============================================================================
    TransportControl(juce::AudioTransportSource &transportSource, bool enabled = true,
        std::function<bool()> recordEnabledFn = nullptr);
    ~TransportControl() override;

    void setEnabled(bool enabled);

    std::function<void(bool)> onLoopingChanged = nullptr;
    std::function<void()> onRecordClicked = nullptr;

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

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

  private:
    enum class TransportState { Stopped, Starting, Playing, Recording, Pausing, Paused, Stopping };
    TransportState state = TransportState::Stopped;
    juce::AudioTransportSource &transportSource;
    bool enabled;

    void createControls();
    void changeState(TransportState newState);
    juce::String getStateLabel();

    void notifyLoopingChanged(bool shouldLoop) const;
    void notifyRecordClicked() const;

    //==============================================================================

    const float buttonImageWidth = 500;
    const float buttonImageHeight = 210;

    std::function<bool()> recordEnabledFn;
    bool recording = false;

    juce::ImageButton startButton;
    juce::ImageButton recordButton;
    juce::ImageButton playButton;
    juce::ImageButton stopButton;
    juce::ImageButton pauseButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;

    juce::Image startButtonImage;
    juce::Image recordButtonOffImage;
    juce::Image recordButtonOnImage;
    juce::Image playButtonOffImage;
    juce::Image playButtonOnImage;
    juce::Image stopButtonImage;
    juce::Image pauseButtonOffImage;
    juce::Image pauseButtonOnImage;

    void drawStartButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawRecordButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawPlayButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawStopButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    void drawPauseButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const;
    static void setButtonImage(juce::ImageButton &button, juce::Image &image);

    void startButtonClicked();
    void recordButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void loopButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControl)
};

} // namespace trackman
