#include "DesktopController.h"
#include "commands/MixerCommands.h"
#include "commands/TrackCommands.h"
#include "common/listutil.h"
#include "controls/mixer/TrackController.h"

DesktopController::DesktopController(juce::AudioFormatManager &formatManager)
    : mixerController(trackList, formatManager), trackListController(trackList), formatManager(formatManager) {
//    mixerPanel.addListener(this);
    mixerController.getMixerPanel().getMasterTrackControl().addListener(this);
    mixerController.addListener((TrackListListener *)this);
    mixerController.addListener((TrackControlListener *)this);
    trackListController.addListener(this);
}

DesktopController::~DesktopController() {
//    for (TrackController *controller : tracks) {
//        delete controller;
//    }
//    tracks.clear();
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
    trackList.eachTrack([this](Track &track) {
        if (track.isSelected()) {
            Command *command = new DeleteTrackCommand(*this, &track);
            commandList.pushCommand(command);
        }
    });

//    if (selected == nullptr) {
//        return;
//    }
//    Command *command = new DeleteTrackCommand(*this, selected);
//    commandList.pushCommand(command);
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

    //    if (selected == nullptr) {
    //        return;
    //    }
    //    Command *command = new DeleteTrackCommand(*this, selected);
    //    commandList.pushCommand(command);
}

void DesktopController::undeleteTrack(Track *track) {
    track->setDeleted(false);
    trackListController.update();
    mixerController.update();
}

//void DesktopController::addTrackController(TrackController *controller) {
//    controller->addListener(this);
//    controller->setListener(&mixerController);
//    controller->getTrackControl().addListener(this);
//    controller->addSource();
//    controller->setVisible(true);
//    mixerController.getMixerPanel().addTrack(controller->getTrackControl());
//    trackListPanel.update();
//}
//
//void DesktopController::removeTrackController(TrackController *controller) {
//    if (selected == controller) {
//        selected = nullptr;
//    }
//    controller->removeSource();
//    controller->removeListener(this);
//    controller->setListener(nullptr);
//    controller->getTrackControl().removeListener(this);
//    controller->setVisible(false);
//    trackListPanel.update();
//    mixerController.getMixerPanel().removeTrack(controller->getTrackControl());
//}
//
//void DesktopController::mixerResized(juce::Rectangle<int> area) {
//    for (TrackController *track : tracks) {
//        if (track->isVisible()) {
//            track->getTrackControl().setBounds(area.removeFromLeft(track->getTrackControl().getWidth()));
//        }
//    }
//}

//void DesktopController::selectionChanged(TrackController *newSelected) {
////    selected = newSelected;
////    for (TrackController *track : tracks) {
////        track->setSelected(track == newSelected);
////    }
//}

void DesktopController::selectionChanged(Track &track, juce::Component *source) {
    trackList.setSelected(track);
    trackListController.repaint();
    mixerController.repaint();
}
