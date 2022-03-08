#include "MixerController.h"
#include "common/listutil.h"
#include "controls/mixer/TrackController.h"

MixerController::MixerController(TrackList &trackList, Mixer &mixer, juce::AudioFormatManager &formatManager)
    : trackList(trackList), mixer(mixer), mixerPanel(trackList, mixer, mixer.getMeterSource()),
      formatManager(formatManager) {
    mixerPanel.getTransportControl().addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
}

MixerController::~MixerController() {
    mixerPanel.getMasterTrackControl().removeListener(this);
    mixerPanel.getTransportControl().removeListener(this);
}

void MixerController::update() {
    mixerPanel.eachTrack([this](TrackControl &track) { track.removeListener(this); });
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->removeListener(this);
    }
    tracks.clear();
    mixerPanel.clear();
    mixer.removeAllSources();
    trackList.eachTrack([this](Track &track) {
        if (track.getSource() != nullptr) {
            mixer.addSource(track.getGain(), track.getSampleRate(), 2);
        }
        auto control = new TrackControl(track);
        control->addListener(this);
        auto controller = new TrackController(track, *control, formatManager);
        controller->addListener(this);
        tracks.push_back(std::unique_ptr<TrackController>(controller));
        mixerPanel.addTrack(control);
    });
    mixerPanel.update();
    mixerPanel.resized();
}

void MixerController::repaint() {
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->repaint();
    }
    mixerPanel.repaint();
}

void MixerController::setMasterLevel(float newLevel) {
    mixer.setMasterLevelGain(newLevel);
    mixerPanel.getMasterTrackControl().setLevel(newLevel);
}

void MixerController::toggleMasterMute() {
    mixer.toggleMasterMute();
    mixerPanel.getMasterTrackControl().update();
}

void MixerController::setLevel(Track &track, float newLevel) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->setLevel(newLevel);
        }
    }
}

void MixerController::toggleMute(Track &track) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->toggleMute(track);
        }
    }
}

void MixerController::onSourceSet() {
    mixer.removeAllSources();
    trackList.eachTrack([this](Track &track) {
        DBG("MixerController::onSourceSet - add track source: " << track.getName());
        if (track.getSource() != nullptr) {
            mixer.addSource(track.getGain(), track.getSampleRate(), 2);
        }
    });
}

void MixerController::loopingChanged(bool shouldLoop) { mixer.setLooping(shouldLoop); }

void MixerController::masterLevelChanged(float newLevel) { mixer.setMasterLevelGain(newLevel); }

void MixerController::masterLevelChangeFinalized(float previousLevel) {
    notifyMasterLevelChangeFinalized(previousLevel);
}

void MixerController::masterMuteToggled() {
    mixer.toggleMasterMute();
    notifyMasterMuteToggled();
}

void MixerController::levelChangeFinalized(Track &track, float previousLevel) {
    notifyLevelChangeFinalized(track, previousLevel);
}

void MixerController::muteToggled(Track &track) { notifyMuteToggled(track); }

void MixerController::selectionChanged(Track &track) { notifySelectionChanged(track); }

void MixerController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void MixerController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void MixerController::notifySelectionChanged(Track &track) {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(track);
    }
}

void MixerController::addListener(MasterTrackListener *listener) {
    if (!listContains(masterTrackListeners, listener)) {
        masterTrackListeners.push_front(listener);
    }
}

void MixerController::removeListener(MasterTrackListener *listener) { masterTrackListeners.remove(listener); }

void MixerController::notifyMasterMuteToggled() {
    for (MasterTrackListener *listener : masterTrackListeners) {
        listener->masterMuteToggled();
    }
}

void MixerController::notifyMasterLevelChangeFinalized(float previousLevel) {
    for (MasterTrackListener *listener : masterTrackListeners) {
        listener->masterLevelChangeFinalized(previousLevel);
    }
}

void MixerController::addListener(TrackControlListener *listener) {
    if (!listContains(trackControlListeners, listener)) {
        trackControlListeners.push_front(listener);
    }
}

void MixerController::removeListener(TrackControlListener *listener) { trackControlListeners.remove(listener); }

void MixerController::notifyLevelChangeFinalized(Track &track, float previousLevel) {
    for (TrackControlListener *listener : trackControlListeners) {
        listener->levelChangeFinalized(track, previousLevel);
    }
}

void MixerController::notifyMuteToggled(Track &track) {
    for (TrackControlListener *listener : trackControlListeners) {
        listener->muteToggled(track);
    }
}
