#include "TransportControl.h"
#include "common/listutil.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource &transportSource, bool enabled)
    : playButtonImage(juce::Image::ARGB, 50, 21, true), stopButtonImage(juce::Image::ARGB, 50, 21, true),
      playButton("play"), stopButton("stop"), transportSource(transportSource), enabled(enabled) {
    transportSource.addChangeListener(this);
    createControls();
    startTimer(20);
}

TransportControl::~TransportControl() {}

void TransportControl::createControls() {

    juce::Graphics g(playButtonImage);
    g.setColour(juce::Colours::steelblue);
    g.fillRect(0, 0, 50, 21);
    g.setColour(juce::Colours::grey.brighter(0.3f));
    g.drawRect(0, 0, 50, 21);
    g.setColour(juce::Colours::white);
    juce::Path p;
    p.addTriangle(21, 5, 21, 17, 32, 11);
    g.fillPath(p);
    playButton.setImages(true, false, false, playButtonImage, 0.9, juce::Colours::transparentWhite, juce::Image(), 1.0,
        juce::Colours::transparentWhite, juce::Image(), 1.0, juce::Colours::transparentWhite);

    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    playButton.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    playButton.setEnabled(enabled);
    addAndMakeVisible(&playButton);

    juce::Graphics g2(stopButtonImage);
    g2.setColour(juce::Colours::steelblue);
    g2.fillRect(0, 0, 50, 21);
    g2.setColour(juce::Colours::grey.brighter(0.3f));
    g2.drawRect(0, 0, 50, 21);
    g2.setColour(juce::Colours::white);
    g2.fillRect(20, 5, 11, 11);
    stopButton.setImages(true, false, false, stopButtonImage, 0.9, juce::Colours::transparentWhite, juce::Image(), 1.0,
        juce::Colours::transparentWhite, juce::Image(), 1.0, juce::Colours::transparentWhite);

    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    stopButton.setEnabled(enabled);
    addAndMakeVisible(&stopButton);

    pauseButton.setButtonText("||");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    pauseButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    pauseButton.setEnabled(enabled);
    addAndMakeVisible(&pauseButton);

    startButton.setButtonText("|<");
    startButton.onClick = [this] { startButtonClicked(); };
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    startButton.setConnectedEdges(juce::Button::ConnectedOnRight);
    startButton.setEnabled(enabled);
    addAndMakeVisible(&startButton);

    loopingToggle.setButtonText("loop");
    loopingToggle.onClick = [this] { loopButtonClicked(); };
    loopingToggle.setEnabled(enabled);
    addAndMakeVisible(&loopingToggle);

    currentPositionLabel.setEnabled(enabled);
    addAndMakeVisible(&currentPositionLabel);
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
            playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
            pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
            transportSource.setPosition(0.0);
            break;

        case TransportState::Starting:
            transportSource.start();
            break;

        case TransportState::Playing:
            playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgreen);
            pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
            break;

        case TransportState::Pausing:
            transportSource.stop();
            break;

        case TransportState::Paused:
            pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::lightgreen);
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

void TransportControl::startButtonClicked() { transportSource.setPosition(0.0); }

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
