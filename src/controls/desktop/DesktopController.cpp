#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackCommands.h"
#include "common/listutil.h"

DesktopController::DesktopController(
    juce::AudioFormatManager &formatManager, MixerPanel &mixerPanel, TrackListPanel &trackListPanel)
    : formatManager(formatManager), mixerPanel(mixerPanel), trackListPanel(trackListPanel) {
    mixerPanel.addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
}

DesktopController::~DesktopController() {
    for (TrackController *track : tracks) {
        delete track;
    }
}

bool DesktopController::canUndo() const { return !commandList.isEmpty(); }

void DesktopController::undoLast() {
    Command *command = commandList.popCommand();
    if (command != nullptr) {
        command->undo();
        delete command;
    }
}

void DesktopController::masterLevelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(getMixer(), previousLevel);
    commandList.pushCommand(command);
}

void DesktopController::levelChangeFinalized(TrackControl &trackControl, float previousLevel) {
    Command *command = new ChangeTrackVolumeCommand(trackControl, previousLevel);
    commandList.pushCommand(command);
}

void DesktopController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Command *command = new AddTrackCommand(*this, name);
    commandList.pushCommand(command);
}

void DesktopController::deleteSelectedTrack() {
    if (selected == nullptr) {
        return;
    }
    Command *command = new DeleteTrackCommand(*this, selected);
    commandList.pushCommand(command);
}

TrackController *DesktopController::addTrack(juce::String name) {
    Track *newTrack = trackList.addTrack(name);
    TrackController *controller = new TrackController(*newTrack, formatManager);
    addTrackController(controller);
    return controller;
}

void DesktopController::addTrackController(TrackController *controller) {
    controller->addListener(this);
    controller->setListener(&mixerPanel);
    controller->getTrackControl().addListener(this);
    controller->addSource();
    tracks.push_back(controller);
    mixerPanel.addTrack(controller->getTrackControl());
    trackListPanel.addTrack(controller->getTrackLaneControl());
    notifyTrackAdded(controller->getTrack());
}

void DesktopController::removeTrackController(TrackController *controller) {
    if (selected == controller) {
        selected = nullptr;
    }
    controller->removeSource();
    controller->removeListener(this);
    controller->setListener(nullptr);
    controller->getTrackControl().removeListener(this);
    trackListPanel.removeTrack(controller->getTrackLaneControl());
    mixerPanel.removeChildComponent(&controller->getTrackControl());
    tracks.remove(controller);
}

void DesktopController::deleteTrackController(TrackController *controller)
{
    trackList.removeTrack(&controller->getTrack());
    delete controller;
}

//TrackController *DesktopController::getController(const Track *track) {
//    TrackController *controller = nullptr;
//    for (TrackController *t : tracks) {
//        if (&t->getTrack() == track) {
//            controller = t;
//            break;
//        }
//    }
//    return controller;
//}

void DesktopController::mixerResized(juce::Rectangle<int> area) {
    for (TrackController *track : tracks) {
        track->getTrackControl().setBounds(area.removeFromLeft(track->getTrackControl().getWidth()));
    }
}

void DesktopController::selectionChanged(TrackController *newSelected) {
    selected = newSelected;
    for (TrackController *track : tracks) {
        track->setSelected(track == newSelected);
    }
}

void DesktopController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void DesktopController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void DesktopController::notifyTrackAdded(Track &track) {
    for (TrackListListener *listener : listeners) {
        listener->trackAdded(track);
    }
}
