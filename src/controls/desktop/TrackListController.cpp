#include "TrackListController.h"
#include "common/listutil.h"

TrackListController::TrackListController(
    juce::AudioFormatManager &formatManager, MixerComponent &mixer, TrackListPanel &trackListPanel)
    : formatManager(formatManager), mixer(mixer), trackListPanel(trackListPanel) {
    mixer.addListener(this);
}

void TrackListController::addNewTrack() {
    juce::String name = juce::String("Track ") + juce::String::formatted(juce::String("%d"), trackList.size() + 1);
    Track *newTrack = trackList.addTrack(name, formatManager);
    newTrack->setListener(&mixer);
    mixer.addAndMakeVisible(newTrack->getTrackControl());
    mixer.resized();
    trackListPanel.addTrack(*newTrack);
    notifyTrackAdded(*newTrack);
}

void TrackListController::mixerResized(juce::Rectangle<int> area) {
    trackList.eachTrack([&area](Track &track) {
        track.getTrackControl().setBounds(area.removeFromLeft(track.getTrackControl().getWidth()));
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
