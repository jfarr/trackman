#include "InstrumentsPanel.h"

namespace trackman {

InstrumentsPanel::InstrumentsPanel(Project &project)
    : project(project), instrumentPlayer(project.getInstrumentPlayer()) {
    setAudioChannels(0, 2);
    //    project.getMidiHandler().onMidiMessage = [this](const MidiMessage &message, const double time) {
    //        midiMessageReceived(message, time);
    //    };
}

InstrumentsPanel::~InstrumentsPanel() { shutdownAudio(); }

// void InstrumentsPanel::midiMessageReceived(const MidiMessage &message, const double time) {
//     DBG("midi " << (message.isNoteOn() ? "on" : "off") << " at " << time);
// }

void InstrumentsPanel::update() {
    removeAllChildren();
    for (InstrumentControl *instrument : instruments) {
        instrument->update();
        addAndMakeVisible(instrument);
    }
    resize();
}

void InstrumentsPanel::resize() {
    double paneHeight = getPaneHeight();
    setSize(panelWidth, topStripHeight + instruments.size() * paneHeight);
    resized();
}

void InstrumentsPanel::paint(Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
}

void InstrumentsPanel::resized() {
    auto area = getLocalBounds();
    auto rightMargin = 3;
    area.removeFromTop(topStripHeight);
    area.removeFromRight(rightMargin);
    for (auto &instrument : instruments) {
        instrument->setBounds(area.removeFromTop(getPaneHeight()));
    }
}

double InstrumentsPanel::getPaneHeight() {
    auto paneHeight = instruments.empty() ? 0 : instruments.back()->getPreferredHeight() * project.getVerticalScale();
    return paneHeight;
}

void InstrumentsPanel::prepareToPlay(int blockSize, double sampleRate) {
    instrumentPlayer.prepareToPlay(blockSize, sampleRate);
}

void InstrumentsPanel::releaseResources() {
    instrumentPlayer.releaseResources();
}

void InstrumentsPanel::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    instrumentPlayer.getNextAudioBlock(bufferToFill);
}

} // namespace trackman
