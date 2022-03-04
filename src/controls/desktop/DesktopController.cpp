#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackListCommands.h"
#include "common/listutil.h"

DesktopController::DesktopController(juce::AudioFormatManager &formatManager)
    : mixerController(trackList, formatManager), trackListController(trackList), formatManager(formatManager) {
    mixerController.getMixerPanel().getMasterTrackControl().addListener(this);
    mixerController.addListener((TrackListListener *)this);
    mixerController.addListener((TrackControlListener *)this);
    trackListController.addListener(this);
}

DesktopController::~DesktopController() {}

bool DesktopController::canUndo() const { return !commandList.isEmpty(); }

void DesktopController::undoLast() { commandList.undoLast(); }

void DesktopController::masterLevelChangeFinalized(float previousLevel) {
    Command *command = new ChangeMasterVolumeCommand(mixerController, previousLevel);
    commandList.pushCommand(command);
}

void DesktopController::levelChangeFinalized(Track &track, float previousLevel) {
    Command *command = new ChangeTrackVolumeCommand(mixerController, track, previousLevel);
    commandList.pushCommand(command);
}

void DesktopController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Command *command = new AddTrackCommand(*this, name);
    commandList.pushCommand(command);
}

void DesktopController::deleteSelectedTrack() {
    trackList.eachTrack([this](Track &track) {
        if (track.isSelected()) {
            Command *command = new DeleteTrackCommand(*this, &track);
            commandList.pushCommand(command);
        }
    });
}

Track *DesktopController::addTrack(juce::String name) {
    auto track = trackList.addTrack(name);
    trackListController.update();
    mixerController.update();
    return track;
}

void DesktopController::deleteTrack(Track *track, bool purge) {
    track->setDeleted(true);
    trackListController.update();
    mixerController.update();
    if (purge) {
        trackList.removeTrack(track);
    }
}

void DesktopController::undeleteTrack(Track *track) {
    track->setDeleted(false);
    trackListController.update();
    mixerController.update();
}

void DesktopController::selectionChanged(Track &track) {
    trackList.setSelected(track);
    trackListController.repaint();
    mixerController.repaint();
}
