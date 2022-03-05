#include "TrackListPanel.h"

TrackListPanel::TrackListPanel(TrackList &trackList, juce::Viewport &viewport, juce::AudioTransportSource &transport,
    juce::AudioFormatManager &formatManager)
    : trackList(trackList), viewport(viewport), positionOverlay(transport), formatManager(formatManager) {

    addAndMakeVisible(positionOverlay);
}

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::update() {
    removeAllChildren();
    addAndMakeVisible(positionOverlay);
    for (TrackLaneControl *lane : lanes) {
        addAndMakeVisible(lane);
    }
    resize();
}

void TrackListPanel::fileDragEnter(const juce::StringArray &files, int x, int y) {
    auto *reader = formatManager.createReaderFor(juce::File(files[0]));
    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    auto length = newSource->getTotalLength() / reader->sampleRate;
    auto width = length * scale;
    addAndMakeVisible(dropBox);
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withWidth(width).withCentre(juce::Point(x, y)));
}

void TrackListPanel::fileDragMove(const juce::StringArray &files, int x, int y) {
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withCentre(juce::Point(x, y)));
}

void TrackListPanel::fileDragExit(const juce::StringArray &files) { removeChildComponent(&dropBox); }

void TrackListPanel::filesDropped(const juce::StringArray &files, int x, int y) { removeChildComponent(&dropBox); }

Track *TrackListPanel::getTrackAtPos(int x, int y) {
    for (TrackLaneControl *lane : lanes) {
        auto area = lane->getBoundsInParent();
        if (area.contains(x, y)) {
            return &lane->getTrack();
        }
    }
    return nullptr;
}

void TrackListPanel::resize() {
    if (!lanes.empty()) {
        setSize(getTrackLaneWidth(), lanes.size() * lanes.back()->getHeight());
    }
    resized();
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto leftPanelWidth = 25;
    auto area = juce::Rectangle(getTrackLaneWidth(), getTrackLaneHeight());
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(lane->getHeight()));
    }
    positionOverlay.setBounds(getLocalBounds().withTrimmedLeft(leftPanelWidth));
}


int TrackListPanel::getTrackLaneWidth() const {
    int trackWidth = trackList.getTotalLength() * scale;
    return std::max(trackWidth, viewport.getWidth());
}

int TrackListPanel::getTrackLaneHeight() const {
    int trackHeight = lanes.size() > 0 ? lanes.size() * lanes.back()->getHeight() : 0;
    return std::max(trackHeight, viewport.getHeight());
}
