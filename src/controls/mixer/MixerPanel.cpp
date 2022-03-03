#include "MixerPanel.h"
#include "common/listutil.h"

MixerPanel::MixerPanel(Mixer &mixer) : mixer(mixer), transportControl(mixer.getTransportSource()) {
    setAudioChannels(0, 2);
    createControls();
    setSize(800, 250);
}

MixerPanel::~MixerPanel() {
    shutdownAudio();
}

void MixerPanel::createControls() {
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
}

void MixerPanel::addTrack(TrackControl &trackControl) {
    addAndMakeVisible(trackControl);
    resized();
}

void MixerPanel::removeTrack(TrackControl &trackControl)
{
    removeChildComponent(&trackControl);
    resized();
}

//==============================================================================
void MixerPanel::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    //    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    mixer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MixerPanel::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    //    transportSource.getNextAudioBlock(bufferToFill);
    mixer.getNextAudioBlock(bufferToFill);
}

void MixerPanel::releaseResources() {
    //    transportSource.releaseResources();
    mixer.releaseResources();
}

//==============================================================================
void MixerPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MixerPanel::resized() {
    auto area = getLocalBounds();
    auto buttonHeight = 35;
    auto transportMargin = 5;
    transportControl.setBounds(area.removeFromTop(buttonHeight).reduced(transportMargin));
    masterTrackControl.setBounds(area.removeFromLeft(masterTrackControl.getWidth()));
    notifyResized(area);
}

void MixerPanel::addListener(MixerPanelListener *listener) {
    if (!listContains(listener, listeners))
        listeners.push_front(listener);
}

void MixerPanel::removeListener(MixerPanelListener *listener) { listeners.remove(listener); }

void MixerPanel::notifyResized(juce::Rectangle<int> area) {
    for (MixerPanelListener *listener : listeners) {
        listener->mixerResized(area);
    }
}
