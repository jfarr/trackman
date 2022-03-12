#include "TransportControl.h"
#include "common/listutil.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource &transportSource, bool enabled)
    : startButtonImage(juce::Image::ARGB, buttonWidth, buttonHeight, true),
      playButtonOffImage(juce::Image::ARGB, buttonWidth, buttonHeight, true),
      playButtonOnImage(juce::Image::ARGB, buttonWidth, buttonHeight, true),
      stopButtonImage(juce::Image::ARGB, buttonWidth, buttonHeight, true),
      pauseButtonOffImage(juce::Image::ARGB, buttonWidth, buttonHeight, true),
      pauseButtonOnImage(juce::Image::ARGB, buttonWidth, buttonHeight, true), startButton("start"), playButton("play"),
      stopButton("stop"), pauseButton("pause"), transportSource(transportSource), enabled(enabled) {
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

void TransportControl::drawStartButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) {
    juce::Graphics g(image);
    juce::Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(0.0, 0.0, buttonWidth, buttonHeight, cornerSize, cornerSize, true, false, true, false);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, juce::PathStrokeType(5.0f));

    g.setColour(juce::Colours::white);
    juce::Path p;
    juce::Point<float> topLeft(210, 50);
    juce::Point<float> bottomLeft(210, 170);
    juce::Point<float> right(320, 110);
    p.addTriangle(topLeft, bottomLeft, right);
    g.fillPath(p);
}

void TransportControl::drawPlayButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) {
    juce::Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonWidth, buttonHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonWidth, buttonHeight, 5.0);
    g.setColour(juce::Colours::white);
    juce::Path p;
    juce::Point<float> topLeft(210, 50);
    juce::Point<float> bottomLeft(210, 170);
    juce::Point<float> right(320, 110);
    p.addTriangle(topLeft, bottomLeft, right);
    g.fillPath(p);
}

void TransportControl::drawStopButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) {
    juce::Graphics g(image);
    g.setColour(bgColor);
    g.fillRect(0.0, 0.0, buttonWidth, buttonHeight);
    g.setColour(borderColor);
    g.drawRect(0.0, 0.0, buttonWidth, buttonHeight, 5.0);
    g.setColour(juce::Colours::white);
    auto left = 200;
    auto top = 50;
    auto iconSize = 110;
    g.fillRect(left, top, iconSize, iconSize);
}

void TransportControl::drawPauseButton(juce::Image &image, juce::Colour bgColor, juce::Colour borderColor) {
    juce::Graphics g(image);
    juce::Path path;
    auto cornerSize = 60.0f;
    g.setColour(bgColor);
    path.addRoundedRectangle(0.0, 0.0, buttonWidth, buttonHeight, cornerSize, cornerSize, false, true, false, true);
    g.fillPath(path);
    g.setColour(borderColor);
    g.strokePath(path, juce::PathStrokeType(5.0f));

    g.setColour(juce::Colours::white);
    float left = 200;
    float top = 50;
    float iconSize = 110;
    g.fillRect(left, top, iconSize / 3, iconSize);
    g.fillRect(left + (iconSize * 2 / 3), top, iconSize / 3, iconSize);
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
            transportSource.setPosition(0.0);
            break;

        case TransportState::Starting:
            transportSource.start();
            break;

        case TransportState::Playing:
            setButtonImage(playButton, playButtonOnImage);
            setButtonImage(pauseButton, pauseButtonOffImage);
            break;

        case TransportState::Pausing:
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
    case TransportState::Paused:
        return juce::String("Paused");
    default:
        return juce::String("");
    }
}

void TransportControl::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &transportSource) {
        if (transportSource.isPlaying()) {
            changeState(TransportState::Playing);
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
