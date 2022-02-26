#include "TransportControl.h"
#include "listutil.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource *transportSource, bool enabled)
{
    this->transportSource = transportSource;
    this->enabled = enabled;
    transportSource->addChangeListener(this);

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
    playButton.setButtonText("play");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    playButton.setEnabled(enabled);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("stop");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    stopButton.setEnabled(false);

    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("pause");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    pauseButton.setEnabled(false);

    addAndMakeVisible(&rewindButton);
    rewindButton.setButtonText("rewind");
    rewindButton.onClick = [this] { rewindButtonClicked(); };
    rewindButton.setColour(juce::TextButton::buttonColourId, juce::Colours::grey);
    rewindButton.setEnabled(enabled);

    addAndMakeVisible(&loopingToggle);
    loopingToggle.setButtonText("loop");
    loopingToggle.onClick = [this] { loopButtonChanged(); };
    loopingToggle.setEnabled(enabled);

    addAndMakeVisible(&currentPositionLabel);
    currentPositionLabel.setEnabled(enabled);
}

void TransportControl::setEnabled(bool enabled)
{
    this->enabled = enabled;
    rewindButton.setEnabled(enabled);
    playButton.setEnabled(enabled && state == TransportState::Stopped);
    pauseButton.setEnabled(enabled);
    loopingToggle.setEnabled(enabled);
    currentPositionLabel.setEnabled(enabled);
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
    rewindButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    playButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    stopButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    pauseButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
    loopingToggle.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
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
            pauseButton.setButtonText("pause");
            stopButton.setEnabled(false);
            transportSource->setPosition(0.0);
            break;

        case TransportState::Starting:
            transportSource->start();
            break;

        case TransportState::Playing:
            pauseButton.setButtonText("pause");
            stopButton.setEnabled(true);
            break;

        case TransportState::Pausing:
            transportSource->stop();
            break;

        case TransportState::Paused:
            pauseButton.setButtonText("resume");
            break;

        case TransportState::Stopping:
            transportSource->stop();
            break;
        }
    }
}

void TransportControl::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == transportSource)
    {
        if (transportSource->isPlaying())
            changeState(TransportState::Playing);
        else if (TransportState::Pausing == state)
            changeState(TransportState::Paused);
        else if ((state == TransportState::Stopping) || (state == TransportState::Playing))
            changeState(TransportState::Stopped);
    }
}

void TransportControl::timerCallback()
{
    juce::RelativeTime position(transportSource->getCurrentPosition());

    auto minutes = ((int)position.inMinutes()) % 60;
    auto seconds = ((int)position.inSeconds()) % 60;
    auto millis = ((int)position.inMilliseconds()) % 1000;

    auto stateText = juce::String((state == TransportState::Stopped) ? " [Stopped]" : (state == TransportState::Paused) ? " [Paused]" : "");
    auto positionString = juce::String::formatted("%02d:%02d:%03d", minutes, seconds, millis) + stateText;

    currentPositionLabel.setText(positionString, juce::dontSendNotification);
}

void TransportControl::playButtonClicked()
{
    updateLoopState(loopingToggle.getToggleState());
    if ((state == TransportState::Stopped) || (state == TransportState::Paused))
        changeState(TransportState::Starting);
    else if (state == TransportState::Playing)
        transportSource->setPosition(0.0);
}

void TransportControl::stopButtonClicked()
{
    if (state == TransportState::Paused)
        changeState(TransportState::Stopped);
    else
        changeState(TransportState::Stopping);
}

void TransportControl::pauseButtonClicked()
{
    if (state == TransportState::Paused)
        changeState(TransportState::Starting);
    else if (state == TransportState::Stopped)
        changeState(TransportState::Paused);
    else
        changeState(TransportState::Pausing);
}

void TransportControl::rewindButtonClicked()
{
    transportSource->setPosition(0.0);
}

void TransportControl::loopButtonChanged()
{
    updateLoopState(loopingToggle.getToggleState());
}

void TransportControl::updateLoopState(bool shouldLoop)
{
    for (std::list<TransportControlListener*>::iterator i = listeners.begin(); i != listeners.end(); ++i)
    {
        TransportControlListener& listener = **i;
        listener.updateLoopState(shouldLoop);
    }
}

void TransportControl::AddListener(TransportControlListener* listener)
{
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void TransportControl::RemoveListener(TransportControlListener* listener)
{
    listeners.remove(listener);
}
