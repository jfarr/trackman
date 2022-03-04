#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackCommands.h"
#include "common/listutil.h"
#include "controls/mixer/TrackController.h"

DesktopController::DesktopController(
    juce::AudioFormatManager &formatManager, Mixer &mixer, MixerPanel &mixerPanel, TrackListPanel &trackListPanel)
    : mixerController(mixer, mixerPanel), trackListController(trackList), formatManager(formatManager), mixer(mixer),
      mixerPanel(mixerPanel), trackListPanel(trackListPanel) {
    mixerPanel.addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
}

DesktopController::~DesktopController() {
    for (TrackController *controller : tracks) {
        delete controller;
    }
    tracks.clear();
}

bool DesktopController::canUndo() const { return !commandList.isEmpty(); }

void DesktopController::undoLast() { commandList.undoLast(); }

void DesktopController::masterLevelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(mixerController, previousLevel);
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
    tracks.push_back(controller);
    addTrackController(controller);
    return controller;
}

void DesktopController::addTrackController(TrackController *controller) {
    controller->addListener(this);
    controller->setListener(&mixerController);
    controller->getTrackControl().addListener(this);
    controller->addSource();
    controller->setVisible(true);
    mixerPanel.addTrack(controller->getTrackControl());
    trackListPanel.update();
}

void DesktopController::removeTrackController(TrackController *controller) {
    if (selected == controller) {
        selected = nullptr;
    }
    controller->removeSource();
    controller->removeListener(this);
    controller->setListener(nullptr);
    controller->getTrackControl().removeListener(this);
    controller->setVisible(false);
    trackListPanel.update();
    mixerPanel.removeTrack(controller->getTrackControl());
}

void DesktopController::mixerResized(juce::Rectangle<int> area) {
    for (TrackController *track : tracks) {
        if (track->isVisible()) {
            track->getTrackControl().setBounds(area.removeFromLeft(track->getTrackControl().getWidth()));
        }
    }
}

void DesktopController::selectionChanged(TrackController *newSelected) {
    selected = newSelected;
    for (TrackController *track : tracks) {
        track->setSelected(track == newSelected);
    }
}
