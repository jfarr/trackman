#include "AudioPlayer.h"
#include "ChildWindow.h"

//==============================================================================
AudioPlayer::AudioPlayer()
{
    transportControl.reset(new TransportControl(&transportSource, false));
    transportControl.get()->AddListener(this);
    addAndMakeVisible(transportControl.get());

    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

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
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPlayer::resized()
{
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    auto transportHeight = 25;
    transportControl.get()->setBounds(0, getHeight() - transportHeight, getWidth() - 20, transportHeight);
}

void AudioPlayer::openButtonClicked()
{
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
        {
            auto file = fc.getResult();

            if (file != juce::File{})
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
        });
}

void AudioPlayer::updateLoopState(bool shouldLoop)
{
    if (readerSource.get() != nullptr)
        readerSource->setLooping(shouldLoop);
}
