#include "TrackListPanel.h"
#include "common/listutil.h"

// InnerTrackPanel::InnerTrackPanel(Project &project, juce::Viewport &viewport, juce::AudioTransportSource &transport)
//     : project(project), viewport(viewport), transport(transport), timeMeter(project) {
//     addAndMakeVisible(timeMeter);
// }

// void InnerTrackPanel::update() {
//     removeAllChildren();
//     addAndMakeVisible(timeMeter);
//     for (TrackLaneControl *lane : lanes) {
//         lane->update();
//         addAndMakeVisible(lane);
//     }
//     resize();
// }
//
// void InnerTrackPanel::resize() {
//     auto w = getPanelWidth();
//     auto h = getPanelHeight();
//     DBG("InnerTrackPanel::resize: " << w << "," << h);
//     setSize(w, h);
//     for (TrackLaneControl *lane : lanes) {
//         lane->resized();
//     }
//     resized();
// }
//
// int InnerTrackPanel::getPanelWidth() const {
//     auto leftPanelWidth = 25;
//     int trackWidth = project.getTrackList().getTotalLengthSeconds() * project.getHorizontalScale();
//     return std::max(trackWidth + leftPanelWidth, viewport.getWidth());
//     //    return 500;
// }
//
// int InnerTrackPanel::getPanelHeight() const {
//     auto topStripWidth = 20;
//     int trackHeight =
//         lanes.size() > 0
//             ? (lanes.size() + 1) * lanes.back()->getPreferredHeight() * project.getVerticalScale() + topStripWidth
//             : topStripWidth;
//     return std::max(trackHeight, viewport.getHeight());
//     //    return 500;
// }
//
// int InnerTrackPanel::getTrackLaneHeight() const {
//     int trackHeight =
//         lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() * project.getVerticalScale() : 0;
//     return trackHeight;
// }

// void InnerTrackPanel::paint(juce::Graphics &g) {
//     g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
//     //    g.fillAll(juce::Colours::yellow);
//
//     auto leftPanelWidth = 25;
//     auto duration = (float)transport.getLengthInSeconds();
//
//     if (duration > 0.0) {
//         auto audioPosition = (float)transport.getCurrentPosition();
//         auto drawPosition = audioPosition * project.getHorizontalScale() + leftPanelWidth;
//
//         g.setColour(juce::Colour{0xff282828});
//         g.drawLine(drawPosition, 0.0f, drawPosition, (float)getHeight(), 1.0f);
//     }
//
//     g.setColour(juce::Colours::lightgrey);
//     g.drawLine(leftPanelWidth, 0, leftPanelWidth, (float)getHeight(), 1.0f);
// }
//
// void InnerTrackPanel::resized() {
//     auto topStripWidth = 20;
//     auto area = getLocalBounds();
//     timeMeter.setBounds(area.removeFromTop(topStripWidth));
//     timeMeter.repaint();
//     auto laneHeight = getTrackLaneHeight() / lanes.size();
//     for (auto &lane : lanes) {
//         lane->setBounds(area.removeFromTop(laneHeight));
//     }
// }

// Track *InnerTrackPanel::getTrackAtPos(int x, int y) {
//     for (TrackLaneControl *lane : lanes) {
//         auto area = lane->getBoundsInParent();
//         if (area.contains(x, y)) {
//             return &lane->getTrack();
//         }
//     }
//     return nullptr;
// }

TrackListPanel::TrackListPanel(Project &project, TrackList &trackList, juce::Viewport &viewport,
    juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : project(project), trackList(trackList), viewport(viewport), transport(transport), formatManager(formatManager) {

    viewport.getHorizontalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    viewport.getHorizontalScrollBar().setAutoHide(false);
    viewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    viewport.getVerticalScrollBar().setAutoHide(false);
    viewport.setScrollBarsShown(true, true);
//    trackListViewport.setScrollBarsShown(false, true);
    viewport.setViewedComponent(this, false);

    //    innerViewport.getVerticalScrollBar().setColour(juce::ScrollBar::thumbColourId, juce::Colours::dimgrey);
    //    innerViewport.getVerticalScrollBar().setAutoHide(false);
    //    innerViewport.setScrollBarsShown(true, false);
    //    innerViewport.setViewedComponent(&innerPanel, false);
    //    addAndMakeVisible(innerViewport);
    update();
    //    resize();
    startTimer(20);
}

TrackListPanel::~TrackListPanel() {}

// void TrackListPanel::update() {
//     //    removeAllChildren();
//     //    addAndMakeVisible(timeMeter);
//     //    addAndMakeVisible(innerViewport);
//     //    for (TrackLaneControl *lane : lanes) {
//     //        lane->update();
//     //        addAndMakeVisible(lane);
//     //    }
//     resize();
//     innerPanel.update();
// }

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

// Track *TrackListPanel::getTrackAtPos(int x, int y) {
//     for (TrackLaneControl *lane : lanes) {
//         auto area = lane->getBoundsInParent();
//         if (area.contains(x, y)) {
//             return &lane->getTrack();
//         }
//     }
//     return nullptr;
// }

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

// void TrackListPanel::resize() {
//     setBounds(juce::Rectangle<int>(innerPanel.getPanelWidth(), innerPanel.getPanelHeight()));
//     innerPanel.resize();
//     //    auto w = getPanelWidth();
//     //    auto h = getPanelHeight();
//     //    setSize(w, h);
//     //    for (TrackLaneControl *lane : lanes) {
//     //        lane->resized();
//     //    }
//     resized();
// }

void TrackListPanel::update() {
    removeAllChildren();
    //    addAndMakeVisible(timeMeter);
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
    auto leftPanelWidth = 25;
    int trackWidth = project.getTrackList().getTotalLengthSeconds() * project.getHorizontalScale();
    return std::max(trackWidth + leftPanelWidth, viewport.getWidth());
    //    return 500;
}

int TrackListPanel::getPanelHeight() const {
    auto topStripWidth = 20;
    int trackHeight =
        lanes.size() > 0
            ? (lanes.size() + 1) * lanes.back()->getPreferredHeight() * project.getVerticalScale()
            : 0;
    return std::max(trackHeight, viewport.getHeight());
    //    return 500;
}

int TrackListPanel::getTrackLaneHeight() const {
    int trackHeight =
        lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() * project.getVerticalScale() : 0;
    return trackHeight;
}

void TrackListPanel::paint(juce::Graphics &g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    //    g.fillAll(juce::Colours::green);
    //
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

// void TrackListPanel::resized() {
//     auto area = getLocalBounds();
//     innerViewport.setBounds(area);
//     //    innerViewport.setBounds(0, 0, 500, 500);
//     //    auto topStripWidth = 20;
//     //    timeMeter.setBounds(area.removeFromTop(topStripWidth));
//     //    timeMeter.repaint();
//     //    auto laneHeight = getTrackLaneHeight() / lanes.size();
//     //    for (auto &lane : lanes) {
//     //        lane->setBounds(area.removeFromTop(laneHeight));
//     //    }
// }

void TrackListPanel::resized() {
    auto topStripWidth = 20;
    auto area = getLocalBounds();
    //    timeMeter.setBounds(area.removeFromTop(topStripWidth));
    //    timeMeter.repaint();
    auto laneHeight = getTrackLaneHeight() / lanes.size();
    for (auto &lane : lanes) {
        lane->setBounds(area.removeFromTop(laneHeight));
    }
}

void TrackListPanel::mouseDown(const juce::MouseEvent &event) {
    Component::mouseDown(event);
    notifySelectionChanged();
}
//
// int TrackListPanel::getPanelWidth() const {
//    int trackWidth = trackList.getTotalLengthSeconds() * project.getHorizontalScale();
//    auto leftPanelWidth = 25;
//    return std::max(trackWidth + leftPanelWidth, viewport.getWidth());
//}
//
// int TrackListPanel::getPanelHeight() const {
//    auto topStripWidth = 20;
//    int trackHeight =
//        lanes.size() > 0
//            ? (lanes.size() + 1) * lanes.back()->getPreferredHeight() * project.getVerticalScale() + topStripWidth
//            : 0;
//    return std::max(trackHeight, viewport.getHeight());
//}
//
// int TrackListPanel::getTrackLaneHeight() const {
//    int trackHeight =
//        lanes.size() > 0 ? lanes.size() * lanes.back()->getPreferredHeight() * project.getVerticalScale() : 0;
//    return trackHeight;
//}

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
