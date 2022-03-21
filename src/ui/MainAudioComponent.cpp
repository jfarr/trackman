#include "MainAudioComponent.h"

namespace trackman {

MainAudioComponent::MainAudioComponent(MainWindow &mainWindow)
    : AudioAppComponent(), desktopController(mainWindow, deviceManager) {
    setSize(800, 600);
    setAudioChannels(0, 2);
    formatManager.registerBasicFormats();
    addAndMakeVisible(desktopController.getDesktopComponent());
}

MainAudioComponent::~MainAudioComponent() { shutdownAudio(); }

//==============================================================================
void MainAudioComponent::prepareToPlay(int blockSize, double sampleRate) {
    desktopController.prepareToPlay(blockSize, sampleRate);
}

void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    desktopController.getNextAudioBlock(bufferToFill);
}

void MainAudioComponent::releaseResources() { desktopController.releaseResources(); }

//==============================================================================
void MainAudioComponent::resized() {
    desktopController.getDesktopComponent().setBounds(getLocalBounds().withTrimmedTop(topStripHeight));
}

} // namespace trackman
