#include "TransportControl.h"
#include "common/listutil.h"

namespace trackman {

//==============================================================================
TransportControl::TransportControl(
    AudioTransportSource &transportSource, bool enabled, function<bool()> recordEnabledFn)
    : startButtonImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      recordButtonOffImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      recordButtonOnImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      playButtonOffImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      playButtonOnImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      stopButtonImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      pauseButtonOffImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      pauseButtonOnImage(Image::ARGB, buttonImageWidth, buttonImageHeight, true), transportSource(transportSource),
      enabled(enabled), recordEnabledFn(recordEnabledFn) {
    transportSource.addChangeListener(this);
    createControls();
    startTimer(20);
}

TransportControl::~TransportControl() {}

void TransportControl::createControls() {

    auto bgColorOff = Colours::steelblue;
    auto bgColorOn = Colour{0xff68ef61};
    auto borderColorOff = Colours::grey.brighter(0.3f);
    auto borderColorOn = Colours::lightgrey;

    drawStartButton(startButtonImage, bgColorOff, borderColorOff);
    setButtonImage(startButton, startButtonImage);

    startButton.onClick = [this] { startButtonClicked(); };
    startButton.setEnabled(enabled);
    addAndMakeVisible(&startButton);

    if (recordEnabledFn != nullptr) {
        drawRecordButton(recordButtonOffImage, Colours::red.withMultipliedLightness(0.75), Colours::dimgrey);
        drawRecordButton(recordButtonOnImage, Colours::red, Colours::grey);
        setButtonImage(recordButton, recordButtonOffImage);

        recordButton.onClick = [this] { recordButtonClicked(); };
        recordButton.setEnabled(recordEnabledFn());
        addAndMakeVisible(&recordButton);
    }

    drawPlayButton(playButtonOffImage, bgColorOff, borderColorOff);
    drawPlayButton(playButtonOnImage, bgColorOn, borderColorOn);
    setButtonImage(playButton, playButtonOffImage);

    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setEnabled(enabled);
    addAndMakeVisible(&playButton);

    drawStopButton(stopButtonImage, bgColorOff, borderColorOff);
    setButtonImage(stopButton, stopButtonImage);

    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setEnabled(enabled);
    addAndMakeVisible(&stopButton);

    drawPauseButton(pauseButtonOffImage, bgColorOff, borderColorOff);
    drawPauseButton(pauseButtonOnImage, bgColorOn, borderColorOn);
    setButtonImage(pauseButton, pauseButtonOffImage);

    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setConnectedEdges(Button::ConnectedOnLeft);
    pauseButton.setEnabled(enabled);
    addAndMakeVisible(&pauseButton);

    loopingToggle.setButtonText("loop");
    loopingToggle.onClick = [this] { loopButtonClicked(); };
    loopingToggle.setEnabled(enabled);
    addAndMakeVisible(&loopingToggle);

    currentPositionLabel.setEnabled(enabled);
    addAndMakeVisible(&currentPositionLabel);
}

void TransportControl::setButtonImage(ImageButton &button, Image &image) {
    button.setImages(false, true, false, image, 1.0, Colours::transparentWhite, Image(), 0.9, Colours::transparentWhite,
        Image(), 1.0, Colour{0x20ffffff});
}

void TransportControl::drawStartButton(Image &image, Colour bgColor, Colour borderColor) const {
    Graphics g(image);
    Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(
        0.0, 0.0, buttonImageWidth, buttonImageHeight, cornerSize, cornerSize, true, false, true, false);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, PathStrokeType(5.0f));

    g.setColour(Colours::white);
    Path p;
    auto iconLeft = 180.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 140.0f;
    auto iconHeight = 120.0f;
    Point<float> left(iconLeft + 40.0, iconTop + iconHeight / 2);
    Point<float> topRight(iconLeft + iconWidth, iconTop);
    Point<float> bottomRight(iconLeft + iconWidth, iconTop + iconHeight);
    p.addTriangle(left, topRight, bottomRight);
    g.fillRect(iconLeft, iconTop, 30.0, iconHeight);
    g.fillPath(p);
}

void TransportControl::drawRecordButton(Image &image, Colour bgColor, Colour borderColor) const {
    Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(Colours::white);
    Path p;
    auto iconLeft = 210.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 110.0f;
    auto iconHeight = 120.0f;
    p.addEllipse(iconLeft, iconTop, iconWidth, iconHeight);
    g.fillPath(p);
}

void TransportControl::drawPlayButton(Image &image, Colour bgColor, Colour borderColor) const {
    Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(Colours::white);
    Path p;
    auto iconLeft = 210.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 110.0f;
    auto iconHeight = 120.0f;
    Point<float> topLeft(iconLeft, iconTop);
    Point<float> bottomLeft(iconLeft, iconTop + iconHeight);
    Point<float> right(iconLeft + iconWidth, iconTop + iconHeight / 2);
    p.addTriangle(topLeft, bottomLeft, right);
    g.fillPath(p);
}

void TransportControl::drawStopButton(Image &image, Colour bgColor, Colour borderColor) const {
    Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(Colours::white);
    auto iconLeft = 200.0f;
    auto iconTop = 50.0f;
    auto iconSize = 110.0f;
    g.fillRect(iconLeft, iconTop, iconSize, iconSize);
}

void TransportControl::drawPauseButton(Image &image, Colour bgColor, Colour borderColor) const {
    Graphics g(image);
    Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(
        0.0, 0.0, buttonImageWidth, buttonImageHeight, cornerSize, cornerSize, false, true, false, true);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, PathStrokeType(5.0f));

    g.setColour(Colours::white);
    float iconLeft = 200.0f;
    float iconTop = 50.0f;
    float iconSize = 110.0f;
    g.fillRect(iconLeft, iconTop, iconSize / 3, iconSize);
    g.fillRect(iconLeft + (iconSize * 2 / 3), iconTop, iconSize / 3, iconSize);
}

void TransportControl::setEnabled(bool isEnabled) {
    enabled = isEnabled;
    startButton.setEnabled(isEnabled);
    playButton.setEnabled(isEnabled);
    stopButton.setEnabled(isEnabled);
    pauseButton.setEnabled(isEnabled);
    loopingToggle.setEnabled(isEnabled);
    currentPositionLabel.setEnabled(isEnabled);
}

void TransportControl::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void TransportControl::resized() {
    auto area = getLocalBounds();

    auto buttonWidth = 50;
    auto loopButtonWidth = 65;
    auto buttonMargin = 2;
    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin).withTrimmedLeft(buttonMargin));
    if (isRecordEnabled()) {
        recordButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin));
    }
    playButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin));
    stopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin));
    pauseButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin));
    loopingToggle.setBounds(area.removeFromLeft(loopButtonWidth).reduced(buttonMargin));
    currentPositionLabel.setBounds(area.reduced(buttonMargin));
}

void TransportControl::changeState(TransportState newState) {
    if (state != newState) {
        state = newState;
        switch (state) {
        case TransportState::Stopped:
            setButtonImage(playButton, playButtonOffImage);
            setButtonImage(pauseButton, pauseButtonOffImage);
            if (isRecordEnabled()) {
                setButtonImage(recordButton, recordButtonOffImage);
                notifyRecordingStopped();
            }
            recording = false;
            transportSource.setPosition(0.0);
            break;

        case TransportState::Starting:
            transportSource.start();
            break;

        case TransportState::Playing:
            setButtonImage(playButton, playButtonOnImage);
            setButtonImage(pauseButton, pauseButtonOffImage);
            if (isRecordEnabled()) {
                if (!recording) {
                    setButtonImage(recordButton, recordButtonOffImage);
                    notifyRecordingPaused();
//                } else {
//                    setButtonImage(recordButton, recordButtonOnImage);
                }
            }
            break;

        case TransportState::Recording:
            setButtonImage(playButton, playButtonOnImage);
            setButtonImage(pauseButton, pauseButtonOffImage);
            setButtonImage(recordButton, recordButtonOnImage);
            notifyRecordingStarted();
            break;

        case TransportState::Pausing:
            notifyRecordingPaused();
            transportSource.stop();
            break;

        case TransportState::Paused:
            setButtonImage(pauseButton, pauseButtonOnImage);
            break;

        case TransportState::Stopping:
            transportSource.stop();
            break;
        }
    }
}

String TransportControl::getStateLabel() {
    switch (state) {
    case TransportState::Stopped:
        return String("Stopped");
    case TransportState::Playing:
        return String("Playing");
    case TransportState::Recording:
        return String("Recording");
    case TransportState::Paused:
        return String("Paused");
    default:
        return String("");
    }
}

void TransportControl::changeListenerCallback(ChangeBroadcaster *source) {
    if (source == &transportSource) {
        if (transportSource.isPlaying()) {
            changeState(recording ? TransportState::Recording : TransportState::Playing);
        } else if (TransportState::Pausing == state) {
            changeState(TransportState::Paused);
        } else if ((state == TransportState::Stopping) || (state == TransportState::Playing)) {
            changeState(TransportState::Stopped);
        }
    }
}

void TransportControl::timerCallback() {
    RelativeTime position(transportSource.getCurrentPosition());

    auto minutes = ((int)position.inMinutes()) % 60;
    auto seconds = ((int)position.inSeconds()) % 60;
    auto millis = ((int)position.inMilliseconds()) % 1000;

    auto length = transportSource.getLengthInSeconds();
    auto lmins = ((int)length / 60) % 60;
    auto lsecs = ((int)length) % 60;
    auto lmillis = (int)(length * 1000.0) % 1000;

    auto stateLabel = getStateLabel();
    auto stateText = stateLabel.isNotEmpty() ? (String(" [") + stateLabel + String("]")) : String("");
    auto positionString = String::formatted("%02d:%02d:%03d", minutes, seconds, millis);
    auto lengthString = String::formatted("%02d:%02d:%03d", lmins, lsecs, lmillis);

    currentPositionLabel.setText(positionString + String(" / ") + lengthString + stateText, dontSendNotification);
}

void TransportControl::startButtonClicked() { transportSource.setPosition(0.0); }

void TransportControl::playButtonClicked() {
    notifyLoopingChanged(loopingToggle.getToggleState());
    if ((state == TransportState::Stopped) || (state == TransportState::Paused)) {
        changeState(TransportState::Starting);
    } else if (state == TransportState::Playing) {
        transportSource.setPosition(0.0);
    }
}

void TransportControl::recordButtonClicked() {
    recording = !recording;
    if ((state == TransportState::Stopped) || (state == TransportState::Paused)) {
        changeState(TransportState::Starting);
    } else if (state == TransportState::Recording) {
        changeState(TransportState::Playing);
    } else if (state == TransportState::Playing) {
        changeState(TransportState::Recording);
    }
}

void TransportControl::stopButtonClicked() {
    if (state == TransportState::Paused) {
        changeState(TransportState::Stopped);
    } else if (state != TransportState::Stopped) {
        changeState(TransportState::Stopping);
    }
}

void TransportControl::pauseButtonClicked() {
    if (state == TransportState::Paused) {
        changeState(TransportState::Starting);
    } else if (state == TransportState::Stopped) {
        changeState(TransportState::Paused);
    } else {
        changeState(TransportState::Pausing);
    }
}

void TransportControl::loopButtonClicked() { notifyLoopingChanged(loopingToggle.getToggleState()); }

void TransportControl::selectionChanged(Track *track) {
    if (recordEnabledFn != nullptr) {
        recordButton.setEnabled(recordEnabledFn());
    }
}

void TransportControl::notifyLoopingChanged(bool shouldLoop) const {
    if (onLoopingChanged != nullptr) {
        onLoopingChanged(shouldLoop);
    }
}

void TransportControl::notifyRecordingStarted() const {
    if (onRecordingStarted != nullptr) {
        onRecordingStarted();
    }
}

void TransportControl::notifyRecordingStopped() const {
    if (onRecordingStopped != nullptr) {
        onRecordingStopped();
    }
}

void TransportControl::notifyRecordingPaused() const {
    if (onRecordingPaused != nullptr) {
        onRecordingPaused();
    }
}

} // namespace trackman
