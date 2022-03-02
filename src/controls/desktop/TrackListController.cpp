#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(
    juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel)
    : formatManager(formatManager), mixer(mixer), trackListPanel(trackListPanel) {
    mixer.addListener(this);
}

TrackListController::~TrackListController() {
    for (TrackController *track : tracks) {
        delete track;
    }
}

Track *TrackListController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Track *newTrack = trackList.addTrack(name);
    TrackController *controller = new TrackController(*newTrack, formatManager);
    controller->addListener(this);
    controller->setListener(&mixer);
    tracks.push_back(controller);
    mixer.addAndMakeVisible(controller->getTrackControl());
    mixer.resized();
    trackListPanel.addTrack(controller->getTrackLaneControl());
    notifyTrackAdded(*newTrack);
    return newTrack;
}

void TrackListController::removeTrack(Track *track) {
    TrackController *controller = getController(track);
    if (controller == nullptr) {
        return;
    }
    trackListPanel.removeTrack(controller->getTrackLaneControl());
    mixer.removeChildComponent(&controller->getTrackControl());
    tracks.remove(controller);
    trackList.removeTrack(track);
    delete controller;
}

TrackController *TrackListController::getController(const Track *track) {
    TrackController *controller = nullptr;
    for (TrackController *t : tracks) {
        if (&t->getTrack() == track) {
            controller = t;
            break;
        }
    }
    return controller;
}

void TrackListController::mixerResized(juce::Rectangle<int> area) {
    for (TrackController *track : tracks) {
        track->getTrackControl().setBounds(area.removeFromLeft(track->getTrackControl().getWidth()));
    }
}

void TrackListController::selectionChanged(TrackController *selected) {
    for (TrackController *track : tracks) {
        track->setSelected(track == selected);
    }
}

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackListController::notifyTrackAdded(Track &track) {
    for (TrackListListener *listener : listeners) {
        listener->trackAdded(track);
    }
}
