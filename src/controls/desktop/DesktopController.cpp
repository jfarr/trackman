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

Track *DesktopController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Track *newTrack = trackList.addTrack(name);
    Command *command = new AddTrackCommand(*this, newTrack);
    commandList.pushCommand(command);

    TrackController *controller = new TrackController(*newTrack, formatManager);
    controller->addListener(this);
    controller->setListener(&mixerPanel);
    controller->getTrackControl().addListener(this);
    tracks.push_back(controller);
    mixerPanel.addAndMakeVisible(controller->getTrackControl());
    mixerPanel.resized();
    trackListPanel.addTrack(controller->getTrackLaneControl());
    notifyTrackAdded(*newTrack);
    return newTrack;
}

void DesktopController::removeTrack(Track *track) {
    TrackController *controller = getController(track);
    if (controller == nullptr) {
        return;
    }
    controller->removeListener(this);
    controller->setListener(nullptr);
    controller->getTrackControl().removeListener(this);
    trackListPanel.removeTrack(controller->getTrackLaneControl());
    mixerPanel.removeChildComponent(&controller->getTrackControl());
    tracks.remove(controller);
    trackList.removeTrack(track);
    delete controller;
}

TrackController *DesktopController::getController(const Track *track) {
    TrackController *controller = nullptr;
    for (TrackController *t : tracks) {
        if (&t->getTrack() == track) {
            controller = t;
            break;
        }
    }
    return controller;
}

void DesktopController::mixerResized(juce::Rectangle<int> area) {
    for (TrackController *track : tracks) {
        track->getTrackControl().setBounds(area.removeFromLeft(track->getTrackControl().getWidth()));
    }
}

void DesktopController::selectionChanged(TrackController *selected) {
    for (TrackController *track : tracks) {
        track->setSelected(track == selected);
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
