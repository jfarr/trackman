#include "TrackListPanel.h"
#include "common/listutil.h"

TrackListPanel::TrackListPanel(Project &project, TrackList &trackList, /*juce::Viewport &viewport,*/
    juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : project(project), trackList(trackList), transport(transport), formatManager(formatManager) {

//    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
//    viewport.getHorizontalScrollBar().setAutoHide(false);
//    viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
//    viewport.getVerticalScrollBar().setAutoHide(false);
//    viewport.setScrollBarsShown(true, true);
//    viewport.setViewedComponent(this, false);

    update();
    startTimer(20);
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
    auto *reader = formatManager.createReaderFor(juce::File(files[0]));
    auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    auto length = newSource->getTotalLength() / reader->sampleRate;
    auto width = length * project.getHorizontalScale();
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
        auto pos = juce::Point<int>(
            dragSourceDetails.localPosition.getX() + dragSourceOffset, dragSourceDetails.localPosition.getY());
        notifySampleDropped(thumbnail, pos.getX(), pos.getY());
        return;
    }
    auto stretchHandle = dynamic_cast<StretchHandle *>(sourceComponent);
    if (stretchHandle != nullptr) {
        auto bounds = stretchHandle->getThumbnail().getBounds();
        notifySampleResized(&stretchHandle->getThumbnail(), dragSourceDetails.localPosition.x - bounds.getX());
        return;
    }
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
    resize();
}

void TrackListPanel::resize() {
    auto w = getPanelWidth();
    auto h = getPanelHeight();
    DBG("TrackListPanel::resize: " << w << "," << h);
    setSize(w, h);
    for (TrackLaneControl *lane : lanes) {
        lane->resized();
    }
    resized();
}

int TrackListPanel::getPanelWidth() const {
//    auto leftPanelWidth = 25;
//    int trackWidth = project.getTrackList().getTotalLengthSeconds() * project.getHorizontalScale();
//    return std::max(trackWidth + leftPanelWidth, viewport.getWidth());
    return 0;
}

int TrackListPanel::getPanelHeight() const {
//    int trackHeight =
//        lanes.size() > 0 ? (lanes.size() + 1) * lanes.back()->getPreferredHeight() * project.getVerticalScale() : 0;
//    return std::max(trackHeight, viewport.getHeight());
    return 0;
}

int TrackListPanel::getTrackLaneHeight() const {
    int trackHeight =
        lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() * project.getVerticalScale() : 0;
    return trackHeight;
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    auto leftPanelWidth = 25;
    auto duration = (float)transport.getLengthInSeconds();

    if (duration > 0.0) {
        auto audioPosition = (float)transport.getCurrentPosition();
        auto drawPosition = audioPosition * project.getHorizontalScale() + leftPanelWidth;

        g.setColour(juce::Colour{0xff282828});
        g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
    }

    g.setColour(juce::Colours::lightgrey);
    g.drawLine(leftPanelWidth, 0, leftPanelWidth, (float)getHeight(), 1.0f);
}

void TrackListPanel::resized() {
    auto area = getLocalBounds();
    auto laneHeight = getTrackLaneHeight() / lanes.size();
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
