#include "TransportControl.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource *transportSource)
{
    this->transportSource = transportSource;

    createControls();
    setSize(300, 225);
    startTimer(20);
}

TransportControl::~TransportControl()
{
}

void TransportControl::createControls()
{
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(false);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    addAndMakeVisible(&loopingToggle);
    loopingToggle.setButtonText("Loop");
    loopingToggle.onClick = [this] { loopButtonChanged(); };

    addAndMakeVisible(&currentPositionLabel);
    currentPositionLabel.setText("Stopped", juce::dontSendNotification);
}

void TransportControl::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TransportControl::resized()
{
    auto area = getLocalBounds();

    auto buttonWidth = 50;
    auto buttonMargin = 2;
    auto loopToggleWidth = 65;
    playButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    stopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    loopingToggle.setBounds(area.removeFromLeft(loopToggleWidth).reduced(buttonMargin));
    currentPositionLabel.setBounds(area.reduced(buttonMargin));
}

void TransportControl::changeState(TransportState newState)
{
    if (state != newState)
    {
        state = newState;

        switch (state)
        {
        case TransportState::Stopped:
            playButton.setButtonText("Play");
            stopButton.setButtonText("Stop");
            stopButton.setEnabled(false);
            transportSource->setPosition(0.0);
            break;

        case TransportState::Starting:
            transportSource->start();
            break;

        case TransportState::Playing:
            playButton.setButtonText("Pause");
            stopButton.setButtonText("Stop");
            stopButton.setEnabled(true);
            break;

        case TransportState::Pausing:
            transportSource->stop();
            break;

        case TransportState::Paused:
            playButton.setButtonText("Resume");
            stopButton.setButtonText("Return to Zero");
            break;

        case TransportState::Stopping:
            transportSource->stop();
            break;
        }
    }
}

void TransportControl::timerCallback()
{
    juce::RelativeTime position(transportSource->getCurrentPosition());

    auto minutes = ((int)position.inMinutes()) % 60;
    auto seconds = ((int)position.inSeconds()) % 60;
    auto millis = ((int)position.inMilliseconds()) % 1000;

    auto positionString = juce::String::formatted("%02d:%02d:%03d", minutes, seconds, millis) + juce::String(transportSource->isPlaying() ? "" : " [Stopped]");

    currentPositionLabel.setText(positionString, juce::dontSendNotification);
}

void TransportControl::playButtonClicked()
{
    updateLoopState(loopingToggle.getToggleState());
    if ((state == TransportState::Stopped) || (state == TransportState::Paused))
        changeState(TransportState::Starting);
    else if (state == TransportState::Playing)
        changeState(TransportState::Pausing);
}

void TransportControl::stopButtonClicked()
{
    if (state == TransportState::Paused)
        changeState(TransportState::Stopped);
    else
        changeState(TransportState::Stopping);
}

void TransportControl::loopButtonChanged()
{
    updateLoopState(loopingToggle.getToggleState());
}

void TransportControl::updateLoopState(bool shouldLoop)
{
    // TODO: notify
}
