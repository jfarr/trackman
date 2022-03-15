#include "TrackLaneControl.h"

TrackLaneControl::TrackLaneControl(Project &project, Track &track, juce::AudioTransportSource &transport)
    : project(project), track(track), transport(transport) {
    createControls();
    setSize(preferredWidth, preferredHeight);
}

TrackLaneControl::~TrackLaneControl() {}

void TrackLaneControl::createControls() {
    if (track.getTrackNumber() > 0) {
        trackLabel.setText("Track " + juce::String(track.getTrackNumber()), juce::dontSendNotification);
        auto font = trackLabel.getFont();
        trackLabel.setFont(trackLabel.getFont().withHeight(11));
        trackLabel.setColour(juce::Label::textColourId, juce::Colour{0xff282828});
        addAndMakeVisible(trackLabel);
    }
}

void TrackLaneControl::addThumbnail(SampleThumbnail *thumbnail) { thumbnails.push_back(thumbnail); }

void TrackLaneControl::update() {
    removeAllChildren();
    trackLabel.setText("Track " + juce::String(track.getTrackNumber()), juce::dontSendNotification);
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

//    auto duration = (float)transport.getLengthInSeconds();
//
//    if (duration > 0.0) {
//        auto audioPosition = (float)transport.getCurrentPosition();
//        auto drawPosition = audioPosition * project.getHorizontalScale() + leftPanelWidth;
//
//        g.setColour(juce::Colour{0xff282828});
//        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
//    }

    g.setColour(juce::Colours::lightgrey);
    g.drawLine(leftPanelWidth, 0, leftPanelWidth, (float)getHeight(), 1.0f);
}

void TrackLaneControl::resized() {
    auto area = getLocalBounds();
    auto leftPanelWidth = 25;
    auto labelHeight = 12;
    auto margin = 3;
    auto scale = project.getHorizontalScale();
    area.removeFromLeft(leftPanelWidth);
    trackLabel.setBounds(area.removeFromTop(labelHeight).withTrimmedTop(margin));
    area.removeFromTop(margin);
    for (SampleThumbnail *thumbnail : thumbnails) {
        auto x = thumbnail->getSample().getStartPos() * scale;
        thumbnail->setBounds(
            x + leftPanelWidth, area.getY(), thumbnail->getSample().getLengthInSeconds() * scale, area.getHeight());
    }
}
