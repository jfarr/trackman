#include "TrackLaneControl.h"

namespace trackman {

TrackLaneControl::TrackLaneControl(Project &project, Track &track, AudioTransportSource &transport)
    : project(project), track(track), transport(transport) {
    createControls();
    update();
    setSize(preferredWidth, preferredHeight);
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

void TrackLaneControl::addSampleThumbnail(SampleThumbnail *thumbnail) { thumbnails.push_back(thumbnail); }

void TrackLaneControl::addNoteCanvas(NoteCanvas *canvas) { canvases.push_back(canvas); }

void TrackLaneControl::update() {
    removeAllChildren();
    trackLabel.setText("Track " + String(track.getTrackNumber()), dontSendNotification);
    addAndMakeVisible(trackLabel);
    for (SampleThumbnail *thumbnail : thumbnails) {
        addAndMakeVisible(thumbnail);
    }
    for (NoteCanvas *canvas : canvases) {
        addAndMakeVisible(canvas);
    }
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
    auto border = 3;
    auto scale = project.getHorizontalScale();
    trackLabel.setBounds(area.removeFromTop(labelHeight).withTrimmedTop(margin));
    area.removeFromTop(margin);
    for (SampleThumbnail *thumbnail : thumbnails) {
        auto &sample = thumbnail->getSample();
        auto x = sample.getStartPosInSeconds() * scale;
        thumbnail->setBounds(x, area.getY(), sample.getLengthInSeconds() * scale, area.getHeight());
    }
    for (NoteCanvas *canvas : canvases) {
        auto &noteRoll = canvas->getNoteRoll();
        auto x = noteRoll.getStartPosInSeconds() * scale - border;
//        DBG("x: " << x);
        auto w = noteRoll.empty() ? 0 : noteRoll.getLengthInSeconds() * scale + 2 * border + 1;
        canvas->setBounds(x, area.getY(), w, area.getHeight());
    }
}

} // namespace trackman
