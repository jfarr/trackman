#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), transportControl(transportSource, false), thumbnailComponent(formatManager),
      positionOverlay(transportSource) {
    setAudioChannels(0, 2);
    createControls();
    setSize(500, 250);
}

AudioPlayer::~AudioPlayer() {
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    fileChooserControl.removeListener(this);
    transportControl.removeListener(this);
    transportSource.setSource(nullptr);
}

void AudioPlayer::createControls() {
    addAndMakeVisible(fileChooserControl);
    addAndMakeVisible(thumbnailComponent);
    addAndMakeVisible(positionOverlay);
    addAndMakeVisible(transportControl);

    fileChooserControl.addListener(this);
    transportControl.addListener(this);
}

//==============================================================================
void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (readerSource.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void AudioPlayer::releaseResources() { transportSource.releaseResources(); }

//==============================================================================
void AudioPlayer::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void AudioPlayer::resized() {
    auto area = getLocalBounds();
    auto buttonHeight = 25;
    auto margin = 2;
    fileChooserControl.setBounds(area.removeFromTop(buttonHeight));
    transportControl.setBounds(area.removeFromBottom(buttonHeight));
    thumbnailComponent.setBounds(area.reduced(margin));
    positionOverlay.setBounds(area.reduced(margin));
}

void AudioPlayer::fileChosen(juce::File file) {
    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
        thumbnailComponent.setSource(file);
        transportControl.setEnabled(true);
    }
}

void AudioPlayer::loopingChanged(bool shouldLoop) {
    if (readerSource.get() != nullptr)
        readerSource->setLooping(shouldLoop);
}
