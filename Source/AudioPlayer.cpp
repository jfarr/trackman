#include "AudioPlayer.h"
#include "ChildWindow.h"

//==============================================================================
AudioPlayer::AudioPlayer()
{
    fileChooserControl.reset(new FileChooserControl());
    fileChooserControl.get()->AddListener(this);
    addAndMakeVisible(fileChooserControl.get());

    transportControl.reset(new TransportControl(&transportSource, false));
    transportControl.get()->AddListener(this);
    addAndMakeVisible(transportControl.get());

    setSize(400, 225);

    formatManager.registerBasicFormats();

    setAudioChannels(2, 2);
}

AudioPlayer::~AudioPlayer()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    transportSource.setSource(nullptr);
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
    //openButton.setBounds(10, 10, getWidth() - 20, 20);
    auto buttonHeight = 25;
    auto margin = 2;
    fileChooserControl.get()->setBounds(0, 0, getWidth() - margin, buttonHeight);
    transportControl.get()->setBounds(0, getHeight() - buttonHeight, getWidth() - margin, buttonHeight);
}

void AudioPlayer::fileChosen(juce::File file)
{
    auto* reader = formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        transportControl.get()->setEnabled(true);
    }
}

void AudioPlayer::updateLoopState(bool shouldLoop)
{
    if (readerSource.get() != nullptr)
        readerSource->setLooping(shouldLoop);
}
