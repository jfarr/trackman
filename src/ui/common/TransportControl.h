#pragma once

#include "model/MidiRecorder.h"
#include "model/TrackList.h"
#include "ui/desktop/TrackListListener.h"
#include <JuceHeader.h>

using namespace std;
using namespace juce;

namespace trackman {

class TransportControl : public Component, public ChangeListener, public Timer, public TrackListListener {
  public:
    //==============================================================================
    TransportControl(
        AudioTransportSource &transportSource, bool enabled = true, function<bool()> recordEnabledFn = nullptr);
    ~TransportControl() override;

    void setEnabled(bool enabled);

    function<void(bool)> onLoopingChanged = nullptr;
    function<void()> onRecordingStarted = nullptr;
    function<void()> onRecordingStopped = nullptr;
    function<void()> onRecordingPaused = nullptr;

    //==============================================================================
    // Component
    void paint(Graphics &g) override;
    void resized() override;

    //==============================================================================
    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster *source) override;

    //==============================================================================
    // Timer
    void timerCallback() override;

    //==============================================================================
    // TrackListListener
    void selectionChanged(Track *track) override;

  private:
    enum class TransportState { Stopped, Starting, Playing, Recording, Pausing, Paused, Stopping };
    TransportState state = TransportState::Stopped;
    AudioTransportSource &transportSource;
    bool enabled;

    void createControls();
    void changeState(TransportState newState);
    String getStateLabel();
    bool isRecordEnabled() const { return recordEnabledFn != nullptr; }

    void notifyLoopingChanged(bool shouldLoop) const;
    void notifyRecordingStarted() const;
    void notifyRecordingStopped() const;
    void notifyRecordingPaused() const;

    //==============================================================================

    const float buttonImageWidth = 500;
    const float buttonImageHeight = 210;

    function<bool()> recordEnabledFn;
    bool recording = false;

    ImageButton startButton;
    ImageButton recordButton;
    ImageButton playButton;
    ImageButton stopButton;
    ImageButton pauseButton;
    ToggleButton loopingToggle;
    Label currentPositionLabel;

    Image startButtonImage;
    Image recordButtonOffImage;
    Image recordButtonOnImage;
    Image playButtonOffImage;
    Image playButtonOnImage;
    Image stopButtonImage;
    Image pauseButtonOffImage;
    Image pauseButtonOnImage;

    void drawStartButton(Image &image, Colour bgColor, Colour borderColor) const;
    void drawRecordButton(Image &image, Colour bgColor, Colour borderColor) const;
    void drawPlayButton(Image &image, Colour bgColor, Colour borderColor) const;
    void drawStopButton(Image &image, Colour bgColor, Colour borderColor) const;
    void drawPauseButton(Image &image, Colour bgColor, Colour borderColor) const;
    static void setButtonImage(ImageButton &button, Image &image);

    void startButtonClicked();
    void recordButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void pauseButtonClicked();
    void loopButtonClicked();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportControl)
};

} // namespace trackman
