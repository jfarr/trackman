#include "TrackLaneControl.h"

namespace trackman {

TrackLaneControl::TrackLaneControl(Project &project, Track &track, AudioTransportSource &transport)
    : project(project), track(track), transport(transport), noteRoll(project, track) {
    setSize(preferredWidth, preferredHeight);
    createControls();
    update();
}

TrackLaneControl::~TrackLaneControl() {}

void TrackLaneControl::createControls() {
    if (track.getTrackNumber() > 0) {
        trackLabel.setText("Track " + String(track.getTrackNumber()), dontSendNotification);
        auto font = trackLabel.getFont();
        trackLabel.setFont(trackLabel.getFont().withHeight(11));
        trackLabel.setColour(Label::textColourId, Colour{0xff282828});
        addAndMakeVisible(trackLabel);
    }
}

void TrackLaneControl::addThumbnail(SampleThumbnail *thumbnail) { thumbnails.push_back(thumbnail); }

void TrackLaneControl::update() {
    removeAllChildren();
    trackLabel.setText("Track " + String(track.getTrackNumber()), dontSendNotification);
    addAndMakeVisible(trackLabel);
    for (SampleThumbnail *thumbnail : thumbnails) {
        addAndMakeVisible(thumbnail);
    }
    addAndMakeVisible(noteRoll);
    resized();
}

void TrackLaneControl::paint(Graphics &g) {
    g.fillAll(track.isSelected() ? Colour{0xff3f5f5f} : Colours::darkslategrey);
    g.setColour(Colours::slategrey);
    g.fillRect(0, getHeight() - 1, getWidth(), 1);
    g.fillRect(0, 0, 1, getHeight());
    g.setColour(Colours::lightgrey);
    g.fillRect(0, 1, 0, getHeight() - 1);
    g.setColour(Colours::lightgrey);
    g.drawLine(0, 0, 0, (float)getHeight(), 1.0f);
}

void TrackLaneControl::resized() {
    auto area = getLocalBounds();
    auto labelHeight = 12;
    auto margin = 3;
    auto scale = project.getHorizontalScale();
    trackLabel.setBounds(area.removeFromTop(labelHeight).withTrimmedTop(margin));
    area.removeFromTop(margin);
    for (SampleThumbnail *thumbnail : thumbnails) {
        auto x = thumbnail->getSample().getStartPos() * scale;
        thumbnail->setBounds(x, area.getY(), thumbnail->getSample().getLengthInSeconds() * scale, area.getHeight());
    }
    noteRoll.setBounds(noteRoll.getBounds().withY(area.getY()).withHeight(area.getHeight()));
    noteRoll.resize();
}

} // namespace trackman
