#include "MixerController.h"
#include "common/listutil.h"
#include "controls/mixer/TrackController.h"

MixerController::MixerController(TrackList &trackList, juce::AudioFormatManager &formatManager)
    : trackList(trackList), mixerPanel(trackList, mixer), formatManager(formatManager) {
    mixerPanel.getTransportControl().addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
}

MixerController::~MixerController() {
    mixerPanel.getMasterTrackControl().removeListener(this);
    mixerPanel.getTransportControl().removeListener(this);
}

void MixerController::update() {
    tracks.clear();
    mixerPanel.clear();
    mixer.removeAllSources();
    trackList.eachTrack([this](Track &track) {
        if (track.getSource() != nullptr) {
            mixer.addSource(track.getSource(), false, track.getSampleRate(), 2);
        }
        auto controller = new TrackController(track, formatManager);
        controller->setListener(this);
        tracks.push_back(std::unique_ptr<TrackController>(controller));
        auto control = new TrackControl(track);
        control->setListener(controller);
        mixerPanel.addTrack(control);
    });
    mixerPanel.resized();
}

void MixerController::repaint() {
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->repaint();
    }
    mixerPanel.repaint();
}

void MixerController::setMasterLevel(float newLevel) {
    mixer.setMasterLevel(newLevel);
    mixerPanel.getMasterTrackControl().setLevel(newLevel);
}

void MixerController::onSourceSet(std::shared_ptr<juce::PositionableAudioSource> newSource,
    std::shared_ptr<juce::PositionableAudioSource> prevSource, const bool deleteWhenRemoved,
    double sourceSampleRateToCorrectFor, int maxNumChannels) {
    //    if (prevSource != nullptr) {
    //        mixer.removeSource(prevSource);
    //    }
    //    if (newSource != nullptr) {
    //        mixer.addSource(newSource, deleteWhenRemoved, sourceSampleRateToCorrectFor);
    //    }
    mixer.removeAllSources();
    trackList.eachTrack([this](Track &track) {
        if (track.getSource() != nullptr) {
            mixer.addSource(track.getSource(), false, track.getSampleRate(), 2);
        }
    });
//    update();
}

void MixerController::loopingChanged(bool shouldLoop) { mixer.setLooping(shouldLoop); }

void MixerController::masterLevelChanged(float newLevel) { mixer.setMasterLevel(newLevel); }

void MixerController::masterMuteToggled() { mixer.toggleMasterMute(); }

void MixerController::levelChangeFinalized(TrackControl &trackControl, float previousLevel) {
    notifyLevelChangeFinalized(trackControl, previousLevel);
}

void MixerController::selectionChanged(Track &track, juce::Component *source) { notifySelectionChanged(track, source); }

void MixerController::addListener(TrackListListener *listener) {
    if (!listContains(listener, trackListListeners)) {
        trackListListeners.push_front(listener);
    }
}

void MixerController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void MixerController::notifySelectionChanged(Track &track, juce::Component *source) {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(track, source);
    }
}

void MixerController::addListener(TrackControlListener *listener) {
    if (!listContains(listener, trackControlListeners)) {
        trackControlListeners.push_front(listener);
    }
}

void MixerController::removeListener(TrackControlListener *listener) { trackControlListeners.remove(listener); }

void MixerController::notifyLevelChangeFinalized(TrackControl &trackControl, float previousLevel) {
    for (TrackControlListener *listener : trackControlListeners) {
        listener->levelChangeFinalized(trackControl, previousLevel);
    }
}
