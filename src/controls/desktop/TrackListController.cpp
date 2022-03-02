#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(
    juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel)
    : formatManager(formatManager), mixer(mixer), trackListPanel(trackListPanel) {
    mixer.addListener(this);
}

TrackListController::~TrackListController() {
    for (std::list<TrackController *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        delete *i;
    }
}

void TrackListController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Track *newTrack = trackList.addTrack(name);
    TrackController *controller = new TrackController(*newTrack, formatManager);
    controller->addListener(this);
    tracks.push_back(controller);
    controller->setListener(&mixer);
    mixer.addAndMakeVisible(controller->getTrackControl());
    mixer.resized();
    trackListPanel.addTrack(controller->getTrackLaneControl());
    notifyTrackAdded(*newTrack);
}

void TrackListController::eachTrack(std::function<void(TrackController &)> f) {
    for (std::list<TrackController *>::iterator i = tracks.begin(); i != tracks.end(); ++i) {
        f(**i);
    }
}

void TrackListController::mixerResized(juce::Rectangle<int> area) {
    eachTrack([&area](TrackController &track) {
        track.getTrackControl().setBounds(area.removeFromLeft(track.getTrackControl().getWidth()));
    });
}

void TrackListController::selectionChanged(TrackController *selected) {
    eachTrack([&selected](TrackController &track) {
        track.setSelected(&track == selected);
    });
}

void TrackListController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackListController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackListController::notifyTrackAdded(Track &track) {
    for (std::list<TrackListListener *>::iterator i = listeners.begin(); i != listeners.end(); ++i) {
        TrackListListener &listener = **i;
        listener.trackAdded(track);
    }
}
