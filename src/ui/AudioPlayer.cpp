#include "AudioPlayer.h"

namespace trackman {

AudioPlayer::AudioPlayer(AudioFormatManager &formatManager)
    : formatManager(formatManager), transportControl(transportSource, false), thumbnailComponent(formatManager),
      positionOverlay(transportSource) {
    setAudioChannels(0, 2);
    createControls();
    setSize(500, 250);
}

AudioPlayer::~AudioPlayer() {
    shutdownAudio();
    fileChooserControl.removeListener(this);
    transportSource.setSource(nullptr);
}

void AudioPlayer::createControls() {
    addAndMakeVisible(fileChooserControl);
    addAndMakeVisible(thumbnailComponent);
    addAndMakeVisible(positionOverlay);
    addAndMakeVisible(transportControl);

    fileChooserControl.addListener(this);
    transportControl.onLoopingChanged = [this](bool shouldLoop) { loopingChanged(shouldLoop); };
    thumbnailComponent.addMouseListener(this, false);
}

//==============================================================================
void AudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void AudioPlayer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    if (readerSource.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    transportSource.getNextAudioBlock(bufferToFill);
}

void AudioPlayer::releaseResources() { transportSource.releaseResources(); }

//==============================================================================
void AudioPlayer::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
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

void AudioPlayer::mouseDown(const MouseEvent &event) {
    auto newEvent = event.getEventRelativeTo(&thumbnailComponent);
    auto duration = transportSource.getLengthInSeconds();

    if (duration > 0.0) {
        auto clickPosition = newEvent.position.x;
        auto audioPosition = (clickPosition / (float)positionOverlay.getWidth()) * duration;
        transportSource.setPosition(audioPosition);
    }
    Component::mouseDown(event);
}

void AudioPlayer::fileChosen(File file) {
    auto *reader = formatManager.createReaderFor(file);
    if (reader != nullptr) {
        auto newSource = make_unique<AudioFormatReaderSource>(reader, true);
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource = move(newSource);
        thumbnailComponent.setSource(file);
        transportControl.setEnabled(true);
    }
}

void AudioPlayer::loopingChanged(bool shouldLoop) {
    if (readerSource != nullptr) {
        readerSource->setLooping(shouldLoop);
    }
}

} // namespace trackman
