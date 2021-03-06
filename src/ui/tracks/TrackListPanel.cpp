#include "TrackListPanel.h"
#include "common/listutil.h"
#include "ui/MainWindow.h"
#include "ui/desktop/DesktopController.h"

namespace trackman {

TrackListPanel::TrackListPanel(
    DesktopController &desktopController, Viewport &viewport, AudioTransportSource &transport)
    : desktopController(desktopController), viewport(viewport), transport(transport), overlay(transport),
      dropBox([this]() { return canDrop(); }) {

    viewport.getHorizontalScrollBar().setColour(ScrollBar::thumbColourId, Colours::dimgrey);
    viewport.getHorizontalScrollBar().setAutoHide(false);
    viewport.getVerticalScrollBar().setColour(ScrollBar::thumbColourId, Colours::dimgrey);
    viewport.getVerticalScrollBar().setAutoHide(false);
    viewport.setScrollBarsShown(true, true);
    viewport.setViewedComponent(this, false);

    update();
    startTimer(20);
}

TrackListPanel::~TrackListPanel() = default;

Track *TrackListPanel::getTrackAtPos(int x, int y) {
    for (TrackLaneControl *lane : lanes) {
        auto area = lane->getBoundsInParent();
        if (area.contains(x, y)) {
            return &lane->getTrack();
        }
    }
    return nullptr;
}

void TrackListPanel::fileDragEnter(const StringArray &files, int x, int y) {
    auto *reader =
        desktopController.getMainWindow().getMainAudioComponent().getFormatManager().createReaderFor(File(files[0]));
    auto newSource = make_unique<AudioFormatReaderSource>(reader, true);
    auto length = (double)newSource->getTotalLength() / reader->sampleRate;
    auto width = (int)(length * desktopController.getProject().getHorizontalScale());
    addAndMakeVisible(dropBox);
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withWidth(width).withCentre(Point(x, y)));
}

void TrackListPanel::fileDragMove(const StringArray &files, int x, int y) {
    auto bounds = dropBox.getLocalBounds();
    dropBox.setBounds(bounds.withCentre(Point(x, y)));
    dragPosition = Point<int>(x, y);
}

void TrackListPanel::fileDragExit(const StringArray &files) { removeChildComponent(&dropBox); }

void TrackListPanel::filesDropped(const StringArray &files, int x, int y) { removeChildComponent(&dropBox); }

bool TrackListPanel::canDrop() {
    auto *track = getTrackAtPos(dragPosition.getX(), dragPosition.getY());
    return track == nullptr || !track->hasMidi();
}

void TrackListPanel::itemDropped(const SourceDetails &dragSourceDetails) {
    auto sourceComponent = dragSourceDetails.sourceComponent.get();
    auto *noteRoll = dynamic_cast<NoteCanvas *>(sourceComponent);
    if (noteRoll != nullptr) {
        auto pos = dragSourceDetails.localPosition;
        notifyNoteRollDropped(noteRoll, pos.getX() - dragSourceOffset, pos.getY());
        return;
    }
    auto *thumbnail = dynamic_cast<SampleThumbnail *>(sourceComponent);
    if (thumbnail != nullptr) {
        auto pos = dragSourceDetails.localPosition;
        notifySampleMoved(thumbnail, pos.getX() - dragSourceOffset, pos.getY());
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
    dragging = true;
    dragSourceOffset = dragSourceDetails.localPosition.getX();
}

void TrackListPanel::dragOperationEnded(const DragAndDropTarget::SourceDetails &dragSourceDetails) {
    dragging = false;
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
    maxWidth = max(maxWidth, w);
    for (auto *lane : lanes) {
        lane->resized();
    }
    resized();
}

void TrackListPanel::timerCallback() {
    if (!dragging && desktopController.getProject().isRecording()) {
        resizePositionOverlay();
    }
}

int TrackListPanel::getPanelWidth() const {
    auto currentPos = desktopController.getProject().getTransport().getTransportSource().getCurrentPosition();
    auto trackListLength = desktopController.getProject().getTrackList().getTotalLengthInSeconds();
    auto scale = desktopController.getProject().getHorizontalScale();
    int trackWidth = (int)(max(currentPos, trackListLength) * scale);
    return max(maxWidth, max(trackWidth, viewport.getWidth()));
}

int TrackListPanel::getPanelHeight() const {
    int trackHeight = lanes.empty() ? 0
                                    : (int)((double)(lanes.size() + 1) * (double)lanes.back()->getPreferredHeight() *
                                            desktopController.getProject().getVerticalScale());
    return max(trackHeight, viewport.getHeight());
}

int TrackListPanel::getTrackLaneHeight() const {
    return lanes.empty() ? 0
                         : (int)((double)lanes.size() * (double)lanes.back()->getPreferredHeight() *
                                 desktopController.getProject().getVerticalScale());
}

void TrackListPanel::paint(Graphics &g) { g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId)); }

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    auto laneHeight = lanes.empty() ? 0 : (int)(getTrackLaneHeight() / lanes.size());
    auto width = max(transport.getLengthInSeconds() * desktopController.getProject().getHorizontalScale(), 2.0);
    overlay.setBounds(area.withWidth(width));
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(laneHeight));
        lane->resized();
    }
}

void TrackListPanel::resizePositionOverlay() {
    auto area = getLocalBounds();
    auto width = max(transport.getLengthInSeconds() * desktopController.getProject().getHorizontalScale(), 2.0);
    overlay.setBounds(area.withWidth(width));
}

void TrackListPanel::mouseDown(const MouseEvent &event) {
    Component::mouseDown(event);
    notifySelectionChanged();
}

void TrackListPanel::notifyNoteRollDropped(NoteCanvas *canvas, int x, int y) const {
    if (onMoveNoteRoll != nullptr) {
        onMoveNoteRoll(canvas->getTrack(), canvas->getNoteRoll(), x, y);
    }
}

void TrackListPanel::notifySampleMoved(SampleThumbnail *thumbnail, int x, int y) const {
    if (onMoveSample != nullptr) {
        onMoveSample(thumbnail->getTrack(), thumbnail->getSample(), x, y);
    }
}

void TrackListPanel::notifySampleResized(SampleThumbnail *thumbnail, int width) const {
    if (onResizeSample != nullptr) {
        onResizeSample(thumbnail->getTrack(), thumbnail->getSample(), width);
    }
}

void TrackListPanel::notifyDragEnded() const {
    if (onDragEnded != nullptr) {
        onDragEnded();
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

} // namespace trackman
