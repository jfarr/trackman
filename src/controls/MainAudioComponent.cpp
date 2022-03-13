#include "MainAudioComponent.h"

MainAudioComponent::MainAudioComponent() {
    setSize(800, 600);
    setAudioChannels(0, 2);
    formatManager.registerBasicFormats();
}

MainAudioComponent::~MainAudioComponent() { shutdownAudio(); }

//==============================================================================
void MainAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {}

void MainAudioComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {}

void MainAudioComponent::releaseResources() {}
