#include "TrackListPanel.h"
#include "common/listutil.h"
#include "ui/MainWindow.h"
#include "ui/desktop/DesktopController.h"

TrackListPanel::TrackListPanel(
    DesktopController &desktopController, juce::Viewport &viewport, juce::AudioTransportSource &transport)
    : desktopController(desktopController), viewport(viewport), transport(transport), overlay(transport) {

    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    viewport.getHorizontalScrollBar().setAutoHide(false);
    viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    viewport.getVerticalScrollBar().setAutoHide(false);
    viewport.setScrollBarsShown(true, true);
    viewport.setViewedComponent(this, false);

    update();
}

TrackListPanel::~TrackListPanel() {}

Track *TrackListPanel::getTrackAtPos(int x, int y) {
    for (TrackLaneControl *lane : lanes) {
        auto area = lane->getBoundsInParent();
        if (area.contains(x, y)) {
            return &lane->getTrack();
        }
    }
    return nullptr;
}

void TrackListPanel::fileDragEnter(const juce::StringArray &files, int x, int y) {
    auto *reader = desktopController.getMainWindow().getMainAudioComponent().getFormatManager().createReaderFor(
        juce::File(files[0]));
    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    auto length = (double)newSource->getTotalLength() / reader->sampleRate;
    auto width = (int)(length * desktopController.getProject().getHorizontalScale());
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

void TrackListPanel::itemDropped(const SourceDetails &dragSourceDetails) {
    auto sourceComponent = dragSourceDetails.sourceComponent.get();
    auto *thumbnail = dynamic_cast<SampleThumbnail *>(sourceComponent);
    if (thumbnail != nullptr) {
        auto pos = dragSourceDetails.localPosition;
        notifySampleDropped(thumbnail, pos.getX() - dragSourceOffset, pos.getY());
        return;
    }
    auto stretchHandle = dynamic_cast<StretchHandle *>(sourceComponent);
    if (stretchHandle != nullptr) {
        auto bounds = stretchHandle->getThumbnail().getBounds();
        notifySampleResized(&stretchHandle->getThumbnail(), dragSourceDetails.localPosition.x - bounds.getX());
        return;
    }
}

void TrackListPanel::dragOperationStarted(const DragAndDropTarget::SourceDetails &dragSourceDetails) {
    dragSourceOffset = dragSourceDetails.localPosition.getX();
}

void TrackListPanel::dragOperationEnded(const DragAndDropTarget::SourceDetails &dragSourceDetails) {
    notifyDragEnded();
}

void TrackListPanel::update() {
    removeAllChildren();
    for (TrackLaneControl *lane : lanes) {
        lane->update();
        addAndMakeVisible(lane);
    }
    addAndMakeVisible(overlay);
    resize();
}

void TrackListPanel::resize() {
    auto w = getPanelWidth();
    auto h = getPanelHeight();
    setSize(w, h);
    for (TrackLaneControl *lane : lanes) {
        lane->resized();
    }
    resized();
}

int TrackListPanel::getPanelWidth() const {
    int trackWidth = (int)(desktopController.getProject().getTrackList().getTotalLengthInSeconds() *
                           desktopController.getProject().getHorizontalScale());
    return std::max(trackWidth, viewport.getWidth());
}

int TrackListPanel::getPanelHeight() const {
    int trackHeight = !lanes.empty() ? (int)((double)(lanes.size() + 1) * (double)lanes.back()->getPreferredHeight() *
                                             desktopController.getProject().getVerticalScale())
                                     : 0;
    return std::max(trackHeight, viewport.getHeight());
}

int TrackListPanel::getTrackLaneHeight() const {
    int trackHeight = !lanes.empty() ? (int)((double)lanes.size() * (double)lanes.back()->getPreferredHeight() *
                                             desktopController.getProject().getVerticalScale())
                                     : 0;
    return trackHeight;
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    auto laneHeight = (int)(getTrackLaneHeight() / lanes.size());
    auto width = std::max(transport.getLengthInSeconds() * desktopController.getProject().getHorizontalScale(), 2.0);
    overlay.setBounds(area.withWidth(width));
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(laneHeight));
    }
}

void TrackListPanel::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
    notifySelectionChanged();
}

void TrackListPanel::addListener(SampleListener *listener) {
    if (!listContains(sampleListeners, listener)) {
        sampleListeners.push_front(listener);
    }
}

void TrackListPanel::removeListener(SampleListener *listener) { sampleListeners.remove(listener); }

void TrackListPanel::notifySampleDropped(SampleThumbnail *thumbnail, int x, int y) {
    for (SampleListener *listener : sampleListeners) {
        listener->sampleMoved(thumbnail->getTrack(), thumbnail->getSample(), x, y);
    }
}

void TrackListPanel::notifySampleResized(SampleThumbnail *thumbnail, int width) {
    for (SampleListener *listener : sampleListeners) {
        listener->sampleResized(thumbnail->getSample(), width);
    }
}

void TrackListPanel::notifyDragEnded() {
    for (SampleListener *listener : sampleListeners) {
        listener->dragEnded();
    }
}

void TrackListPanel::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void TrackListPanel::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void TrackListPanel::notifySelectionChanged() {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(nullptr);
    }
}
