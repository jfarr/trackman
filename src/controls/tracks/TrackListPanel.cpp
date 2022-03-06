#include "TrackListPanel.h"
#include "common/listutil.h"

TrackListPanel::TrackListPanel(TrackList &trackList, juce::Viewport &viewport, juce::AudioTransportSource &transport,
    juce::AudioFormatManager &formatManager)
    : trackList(trackList), viewport(viewport), transport(transport), formatManager(formatManager) {
    createControls();
    resize();
    startTimer(20);
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
    dragSourceDetails.sourceComponent->getParentComponent()->removeChildComponent(dragSourceDetails.sourceComponent);
}

void TrackListPanel::dragOperationEnded(const DragAndDropTarget::SourceDetails &dragSourceDetails) {
    notifyDragEnded();
}

void TrackListPanel::resize() {
    auto topStripWidth = 20;
    setSize(getTrackLaneWidth(), getTrackLaneHeight());
    for (TrackLaneControl *lane : lanes) {
        lane->resized();
    }
    resized();
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    auto leftPanelWidth = 25;
    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        auto drawPosition = audioPosition * scale + leftPanelWidth;

        g.setColour(juce::Colour{0xff282828});
        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
    }

    g.setColour(juce::Colours::lightgrey);
    g.drawLine(leftPanelWidth, 0, leftPanelWidth, (float)getHeight(), 1.0f);
}

void TrackListPanel::resized() {
    auto topStripWidth = 20;
    auto area = getLocalBounds();
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
    auto topStripWidth = 20;
    int trackHeight = lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() : 0;
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
        listener->sampleMoved(thumbnail->getSample(), pos);
    }
}

void TrackListPanel::notifyDragEnded() {
    for (SampleListener *listener : listeners) {
        listener->dragEnded();
    }
}
