#include "AudioPlayer.h"
#include "ChildWindow.h"

//==============================================================================
AudioPlayer::AudioPlayer(juce::AudioFormatManager* formatManager) : thumbnailComponent(*formatManager), positionOverlay(transportSource)
{
    this->formatManager = formatManager;
    setAudioChannels(2, 2);

    fileChooserControl.reset(new FileChooserControl());
    fileChooserControl.get()->AddListener(this);
    addAndMakeVisible(fileChooserControl.get());

    addAndMakeVisible(&thumbnailComponent);
    addAndMakeVisible(&positionOverlay);

    transportControl.reset(new TransportControl(&transportSource, false));
    transportControl.get()->AddListener(this);
    addAndMakeVisible(transportControl.get());

    setSize(400, 225);
}

AudioPlayer::~AudioPlayer()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    transportSource.setSource(nullptr);
    if (fileChooserControl.get() != nullptr)
        fileChooserControl.get()->RemoveListener(this);
    if (transportControl.get() != nullptr)
        transportControl.get()->RemoveListener(this);
}

//==============================================================================
void AudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioPlayer::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void AudioPlayer::releaseResources()
{
    transportSource.releaseResources();
}


//==============================================================================
void AudioPlayer::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPlayer::resized()
{
    auto area = getLocalBounds();
    auto buttonHeight = 25;
    auto margin = 2;
    fileChooserControl.get()->setBounds(area.removeFromTop(buttonHeight));
    transportControl.get()->setBounds(area.removeFromBottom(buttonHeight));
    thumbnailComponent.setBounds(area.reduced(margin));
    positionOverlay.setBounds(area.reduced(margin));
}

void AudioPlayer::fileChosen(juce::File file)
{
    auto* reader = formatManager->createReaderFor(file);

    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        thumbnailComponent.setSource(file);
        transportControl.get()->setEnabled(true);
    }
}

void AudioPlayer::updateLoopState(bool shouldLoop)
{
    if (readerSource.get() != nullptr)
        readerSource->setLooping(shouldLoop);
}
