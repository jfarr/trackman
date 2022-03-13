#include "MainAudioComponent.h"

MainAudioComponent::MainAudioComponent(MainWindow &mainWindow)
    : juce::AudioAppComponent(), desktopController(mainWindow, *this, deviceManager.getAudioDeviceSetup().sampleRate) {
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

void MainAudioComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    desktopController.getNextAudioBlock(bufferToFill);
}

void MainAudioComponent::releaseResources() { desktopController.releaseResources(); }

//==============================================================================
void MainAudioComponent::resized() {
    desktopController.getDesktopComponent().setBounds(getLocalBounds().withTrimmedTop(topStripHeight));
}
