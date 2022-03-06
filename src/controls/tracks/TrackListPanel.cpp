#include "TrackListPanel.h"
#include "common/listutil.h"

TrackListPanel::TrackListPanel(TrackList &trackList, juce::Viewport &viewport, juce::AudioTransportSource &transport,
    juce::AudioFormatManager &formatManager)
    : trackList(trackList), viewport(viewport), formatManager(formatManager) {
    createControls();
}

TrackListPanel::~TrackListPanel() {}

void TrackListPanel::createControls() { addAndMakeVisible(timeMeter); }

void TrackListPanel::update() {
    removeAllChildren();
    addAndMakeVisible(timeMeter);
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

void TrackListPanel::itemDropped(const SourceDetails &dragSourceDetails) {
    auto *thumbnail = dynamic_cast<SampleThumbnail *>(dragSourceDetails.sourceComponent.get());
    if (thumbnail == nullptr) {
        return;
    }
    auto pos = juce::Point<int>(
        dragSourceDetails.localPosition.getX() + dragSourceOffset, dragSourceDetails.localPosition.getY());
    notifySampleDropped(thumbnail, pos);
}

void TrackListPanel::dragOperationStarted(const DragAndDropTarget::SourceDetails &dragSourceDetails) {
    dragSourceOffset = dragSourceDetails.sourceComponent->getWidth() / 2 - dragSourceDetails.localPosition.getX();
}

void TrackListPanel::resize() {
    auto topStripWidth = 20;
    if (!lanes.empty()) {
        setSize(getTrackLaneWidth(), lanes.size() * lanes.back()->getHeight() + topStripWidth);
        for (TrackLaneControl *lane : lanes) {
            lane->resized();
        }
    } else {
        setSize(getTrackLaneWidth(), topStripWidth);
    }
    resized();
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto topStripWidth = 20;
    auto area = juce::Rectangle(getTrackLaneWidth(), getTrackLaneHeight());
    timeMeter.setBounds(area.removeFromTop(topStripWidth));
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(lane->getHeight()));
    }
}

int TrackListPanel::getTrackLaneWidth() const {
    int trackWidth = trackList.getTotalLengthSeconds() * scale;
    auto leftPanelWidth = 25;
    return std::max(trackWidth + leftPanelWidth, viewport.getWidth());
}

int TrackListPanel::getTrackLaneHeight() const {
    int trackHeight = lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() : 0;
    auto topStripWidth = 20;
    return std::max(trackHeight + topStripWidth, viewport.getHeight());
}

void TrackListPanel::addListener(SampleListener *listener) {
    if (!listContains(listeners, listener)) {
        listeners.push_front(listener);
    }
}

void TrackListPanel::removeListener(SampleListener *listener) { listeners.remove(listener); }

void TrackListPanel::notifySampleDropped(SampleThumbnail *thumbnail, juce::Point<int> pos) {
    for (SampleListener *listener : listeners) {
        listener->sampleDropped(thumbnail, pos);
    }
}
