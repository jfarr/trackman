#include "TransportControl.h"
#include "listutil.h"

//==============================================================================
TransportControl::TransportControl(juce::AudioTransportSource& transportSource, bool enabled)
    : transportSource(transportSource), enabled(enabled)
{
    transportSource.addChangeListener(this);
    createControls();
    startTimer(20);
}

TransportControl::~TransportControl()
{
}

void TransportControl::createControls()
{
    addAndMakeVisible(&playButton);
    playButton.setButtonText(">");
    playButton.onClick = [this] { playButtonClicked(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    playButton.setEnabled(enabled);

    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("[]");
    stopButton.onClick = [this] { stopButtonClicked(); };
    stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    stopButton.setEnabled(enabled);

    addAndMakeVisible(&pauseButton);
    pauseButton.setButtonText("||");
    pauseButton.onClick = [this] { pauseButtonClicked(); };
    pauseButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    pauseButton.setEnabled(enabled);

    addAndMakeVisible(&startButton);
    startButton.setButtonText("|<");
    startButton.onClick = [this] { startButtonClicked(); };
    startButton.setColour(juce::TextButton::buttonColourId, juce::Colours::steelblue);
    startButton.setEnabled(enabled);

    addAndMakeVisible(&loopingToggle);
    loopingToggle.setButtonText("loop");
    loopingToggle.onClick = [this] { loopButtonChanged(); };
    loopingToggle.setEnabled(enabled);

    addAndMakeVisible(&currentPositionLabel);
    currentPositionLabel.setEnabled(enabled);
}

void TransportControl::setEnabled(bool isEnabled)
{
    enabled = isEnabled;
    startButton.setEnabled(isEnabled);
    playButton.setEnabled(isEnabled);
    stopButton.setEnabled(isEnabled);
    pauseButton.setEnabled(isEnabled);
    loopingToggle.setEnabled(isEnabled);
    currentPositionLabel.setEnabled(isEnabled);
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
    startButton.setBounds(area.removeFromLeft(buttonWidth).reduced(buttonMargin));
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

juce::String TransportControl::getStateLabel()
{
    switch (state)
    {
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

void TransportControl::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState(TransportState::Playing);
        else if (TransportState::Pausing == state)
            changeState(TransportState::Paused);
        else if ((state == TransportState::Stopping) || (state == TransportState::Playing))
            changeState(TransportState::Stopped);
    }
}

void TransportControl::timerCallback()
{
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

    currentPositionLabel.setText(positionString + juce::String(" / ") + lengthString + stateText, juce::dontSendNotification);
}

void TransportControl::playButtonClicked()
{
    updateLoopState(loopingToggle.getToggleState());
    if ((state == TransportState::Stopped) || (state == TransportState::Paused))
        changeState(TransportState::Starting);
    else if (state == TransportState::Playing)
        transportSource.setPosition(0.0);
}

void TransportControl::stopButtonClicked()
{
    if (state == TransportState::Paused)
        changeState(TransportState::Stopped);
    else if (state != TransportState::Stopped)
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

void TransportControl::startButtonClicked()
{
    transportSource.setPosition(0.0);
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

void TransportControl::addListener(TransportControlListener* listener)
{
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void TransportControl::removeListener(TransportControlListener* listener)
{
    listeners.remove(listener);
}
