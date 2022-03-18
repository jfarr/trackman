#include "TransportControl.h"
#include "common/listutil.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource &transportSource, bool enabled, MidiRecorder *recorder, TrackList *trackList)
    : startButtonImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      recordButtonOffImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      recordButtonOnImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      playButtonOffImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      playButtonOnImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      stopButtonImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      pauseButtonOffImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true),
      pauseButtonOnImage(juce::Image::ARGB, buttonImageWidth, buttonImageHeight, true), startButton("start"),
      recordButton("record"), playButton("play"), stopButton("stop"), pauseButton("pause"),
      transportSource(transportSource), enabled(enabled), recorder(recorder), trackList(trackList) {
    transportSource.addChangeListener(this);
    createControls();
    startTimer(20);
}

TransportControl::~TransportControl() {}

void TransportControl::createControls() {

    auto bgColorOff = juce::Colours::steelblue;
    auto bgColorOn = juce::Colour{0xff68ef61};
    auto borderColorOff = juce::Colours::grey.brighter(0.3f);
    auto borderColorOn = juce::Colours::lightgrey;

    drawStartButton(startButtonImage, bgColorOff, borderColorOff);
    setButtonImage(startButton, startButtonImage);

    startButton.onClick = [this] { startButtonClicked(); };
    startButton.setEnabled(enabled);
    addAndMakeVisible(&startButton);

    if (recorder != nullptr) {
        drawRecordButton(recordButtonOffImage, juce::Colours::red.withMultipliedLightness(0.75), juce::Colours::dimgrey);
        drawRecordButton(recordButtonOnImage, juce::Colours::red, juce::Colours::grey);
        setButtonImage(recordButton, recordButtonOffImage);

        recordButton.onClick = [this] { recordButtonClicked(); };
        recordButton.setEnabled(trackList->canRecord());
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
    pauseButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    pauseButton.setEnabled(enabled);
    addAndMakeVisible(&pauseButton);

    loopingToggle.setButtonText("loop");
    loopingToggle.onClick = [this] { loopButtonClicked(); };
    loopingToggle.setEnabled(enabled);
    addAndMakeVisible(&loopingToggle);

    currentPositionLabel.setEnabled(enabled);
    addAndMakeVisible(&currentPositionLabel);
}

void TransportControl::setButtonImage(juce::ImageButton &button, juce::Image &image) {
    button.setImages(false, true, false, image, 1.0, juce::Colours::transparentWhite, juce::Image(), 0.9,
        juce::Colours::transparentWhite, juce::Image(), 1.0, juce::Colour{0x20ffffff});
}

void TransportControl::drawStartButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const {
    juce::Graphics g(image);
    juce::Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(
        0.0, 0.0, buttonImageWidth, buttonImageHeight, cornerSize, cornerSize, true, false, true, false);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, juce::PathStrokeType(5.0f));

    g.setColour(juce::Colours::white);
    juce::Path p;
    auto iconLeft = 180.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 140.0f;
    auto iconHeight = 120.0f;
    juce::Point<float> left(iconLeft + 40.0, iconTop + iconHeight / 2);
    juce::Point<float> topRight(iconLeft + iconWidth, iconTop);
    juce::Point<float> bottomRight(iconLeft + iconWidth, iconTop + iconHeight);
    p.addTriangle(left, topRight, bottomRight);
    g.fillRect(iconLeft, iconTop, 30.0, iconHeight);
    g.fillPath(p);
}

void TransportControl::drawRecordButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const {
    juce::Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(juce::Colours::white);
    juce::Path p;
    auto iconLeft = 210.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 110.0f;
    auto iconHeight = 120.0f;
    p.addEllipse(iconLeft, iconTop, iconWidth, iconHeight);
    g.fillPath(p);
}

void TransportControl::drawPlayButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const {
    juce::Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(juce::Colours::white);
    juce::Path p;
    auto iconLeft = 210.0f;
    auto iconTop = 50.0f;
    auto iconWidth = 110.0f;
    auto iconHeight = 120.0f;
    juce::Point<float> topLeft(iconLeft, iconTop);
    juce::Point<float> bottomLeft(iconLeft, iconTop + iconHeight);
    juce::Point<float> right(iconLeft + iconWidth, iconTop + iconHeight / 2);
    p.addTriangle(topLeft, bottomLeft, right);
    g.fillPath(p);
}

void TransportControl::drawStopButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const {
    juce::Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonImageWidth, buttonImageHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonImageWidth, buttonImageHeight, 5.0);
    g.setColour(juce::Colours::white);
    auto iconLeft = 200.0f;
    auto iconTop = 50.0f;
    auto iconSize = 110.0f;
    g.fillRect(iconLeft, iconTop, iconSize, iconSize);
}

void TransportControl::drawPauseButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) const {
    juce::Graphics g(image);
    juce::Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(
        0.0, 0.0, buttonImageWidth, buttonImageHeight, cornerSize, cornerSize, false, true, false, true);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, juce::PathStrokeType(5.0f));

    g.setColour(juce::Colours::white);
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

void TransportControl::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TransportControl::resized() {
    auto area = getLocalBounds();

    auto buttonWidth = 50;
    auto loopButtonWidth = 65;
    auto buttonMargin = 2;
    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(0, buttonMargin).withTrimmedLeft(buttonMargin));
    if (recorder != nullptr) {
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
            if (recorder->isRecording()) {
                setButtonImage(recordButton, recordButtonOffImage);
                recorder->stopRecording();
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
            break;

        case TransportState::Recording:
            setButtonImage(playButton, playButtonOnImage);
            setButtonImage(pauseButton, pauseButtonOffImage);
            setButtonImage(recordButton, recordButtonOnImage);
            recorder->startRecording();
            break;

        case TransportState::Pausing:
            recorder->stopRecording();
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

juce::String TransportControl::getStateLabel() {
    switch (state) {
    case TransportState::Stopped:
        return juce::String("Stopped");
    case TransportState::Playing:
        return juce::String("Playing");
    case TransportState::Recording:
        return juce::String("Recording");
    case TransportState::Paused:
        return juce::String("Paused");
    default:
        return juce::String("");
    }
}

void TransportControl::changeListenerCallback(juce::ChangeBroadcaster *source) {
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
    juce::RelativeTime position(transportSource.getCurrentPosition());

    auto minutes = ((int)position.inMinutes()) % 60;
    auto seconds = ((int)position.inSeconds()) % 60;
    auto millis = ((int)position.inMilliseconds()) % 1000;

    auto length = transportSource.getLengthInSeconds();
    auto lmins = ((int)length / 60) % 60;
    auto lsecs = ((int)length) % 60;
    auto lmillis = (int)(length * 1000.0) % 1000;

    auto stateLabel = getStateLabel();
    auto stateText = stateLabel.isNotEmpty() ? (juce::String(" [") + stateLabel + juce::String("]")) : juce::String("");
    auto positionString = juce::String::formatted("%02d:%02d:%03d", minutes, seconds, millis);
    auto lengthString = juce::String::formatted("%02d:%02d:%03d", lmins, lsecs, lmillis);

    currentPositionLabel.setText(
        positionString + juce::String(" / ") + lengthString + stateText, juce::dontSendNotification);
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
    if ((state == TransportState::Stopped) || (state == TransportState::Paused)) {
        changeState(TransportState::Starting);
    } else if (state == TransportState::Playing) {
        if (recorder->isRecording()) {
            recorder->stopRecording();
            setButtonImage(recordButton, recordButtonOffImage);
        } else {
            recorder->startRecording();
            setButtonImage(recordButton, recordButtonOnImage);
        }
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
    recordButton.setEnabled(trackList->canRecord());
}

void TransportControl::addListener(TransportControlListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void TransportControl::removeListener(TransportControlListener *listener) { listeners.remove(listener); }

void TransportControl::notifyLoopingChanged(bool shouldLoop) {
    for (TransportControlListener *listener : listeners) {
        listener->loopingChanged(shouldLoop);
    }
}

void TransportControl::notifyRecordingStopped() {
    for (TransportControlListener *listener : listeners) {
        listener->recordingStopped();
    }
}
