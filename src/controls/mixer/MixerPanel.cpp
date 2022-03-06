#include "MixerPanel.h"
#include "common/listutil.h"

MixerPanel::MixerPanel(TrackList &trackList, Mixer &mixer)
    : masterTrackControl(mixer), trackList(trackList), mixer(mixer), transportControl(mixer.getTransportSource()) {
    createControls();
    setSize(800, 250);
}

MixerPanel::~MixerPanel() {}

void MixerPanel::createControls() {
    addAndMakeVisible(transportControl);
    addAndMakeVisible(masterTrackControl);
}

void MixerPanel::clear() {
    tracks.clear();
    removeAllChildren();
    createControls();
}

void MixerPanel::update() { masterTrackControl.update(); }

void MixerPanel::addTrack(TrackControl *trackControl) {
    tracks.push_back(std::unique_ptr<TrackControl>(trackControl));
    addAndMakeVisible(trackControl);
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
    for (auto &track : tracks) {
        track->setBounds(area.removeFromLeft(track->getWidth()));
    }
}
