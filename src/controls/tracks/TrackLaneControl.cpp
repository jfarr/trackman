#include "TrackLaneControl.h"

TrackLaneControl::TrackLaneControl(Track &track) : track(track) {
    createControls();
    setSize(800, 100);
}

TrackLaneControl::~TrackLaneControl() {}

void TrackLaneControl::createControls() {
    trackLabel.setText(track.getName(), juce::dontSendNotification);
    trackLabel.setColour(juce::Label::textColourId, juce::Colour{0xff282828});
    addAndMakeVisible(trackLabel);
}

void TrackLaneControl::addThumbnail(SampleThumbnail *thumbnail) { thumbnails.push_back(thumbnail); }

void TrackLaneControl::update() {
    removeAllChildren();
    addAndMakeVisible(trackLabel);
    for (SampleThumbnail *thumbnail : thumbnails) {
        addAndMakeVisible(thumbnail);
    }
    resized();
}

void TrackLaneControl::paint(juce::Graphics &g) {
    auto leftPanelWidth = 25;
    g.fillAll(track.isSelected() ? juce::Colour{0xff3f5f5f} : juce::Colours::darkslategrey);
    g.setColour(juce::Colours::slategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    g.fillRect(leftPanelWidth, 0, 1, getHeight());
}

void TrackLaneControl::resized() {
    auto area = getLocalBounds();
    auto leftPanelWidth = 25;
    auto labelHeight = 15;
    auto margin = 2;
    area.removeFromLeft(leftPanelWidth);
    trackLabel.setBounds(area.removeFromTop(labelHeight).reduced(margin));
    for (SampleThumbnail *thumbnail : thumbnails) {
        auto x = thumbnail->getSample().getStartPos() * scale;
        thumbnail->setBounds(
            x + leftPanelWidth, area.getY(), thumbnail->getSample().getLength() * scale, thumbnail->getHeight());
    }
}
