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
    function<void()> onRecordClicked = nullptr;
    function<void()> onRecordStarted = nullptr;
    function<void()> onRecordStopped = nullptr;
    function<void()> onRecordPaused = nullptr;

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
//    void notifyRecordClicked() const;
    void notifyRecordStarted() const;
    void notifyRecordStopped() const;
    void notifyRecordPaused() const;

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
