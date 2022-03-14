#include "MixerController.h"
#include "ui/desktop/DesktopController.h"
#include "ui/mixer/TrackController.h"

MixerController::MixerController(DesktopController &desktopController)
    : desktopController(desktopController), trackList(desktopController.getProject().getTrackList()),
      mixer(desktopController.getProject().getMixer()), mixerPanel(desktopController, mixer.getMeterSource()),
      trackPanel(mixerViewport) {

    mixerViewport.setViewedComponent(&trackPanel, false);
    mixerPanel.getTransportControl().addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
    update();
}

MixerController::~MixerController() {
    // TODO: remove this listener, just call the controller directly
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->removeListener((TrackListListener *)this);
    }
    mixerPanel.getMasterTrackControl().removeListener(this);
    mixerPanel.getTransportControl().removeListener(this);
}

void MixerController::update() {
    tracks.clear();
    trackPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto controller = new TrackController(desktopController, track);
        tracks.push_back(std::unique_ptr<TrackController>(controller));
        trackPanel.addTrack(&controller->getTrackControl());
    });
    trackPanel.update();
}

void MixerController::repaint() {
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->repaint();
    }
    mixerPanel.repaint();
    trackPanel.repaint();
}

void MixerController::setMasterLevel(float newLevel) {
    mixer.setMasterLevelGain(newLevel);
    mixerPanel.getMasterTrackControl().setLevel(newLevel);
}

void MixerController::setMasterMute(bool newMute) {
    mixer.setMasterMute(newMute);
    mixerPanel.getMasterTrackControl().update();
}

void MixerController::setLevel(Track &track, float newLevel) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->setLevel(newLevel);
        }
    }
}

void MixerController::setMute(Track &track, bool newMute) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->setMute(track, newMute);
        }
    }
}

void MixerController::setSolo(Track &track, bool newSolo) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->setSolo(track, newSolo);
        }
    }
}

void MixerController::loopingChanged(bool shouldLoop) { mixer.setLooping(shouldLoop); }

void MixerController::masterLevelChanged(float newLevel) { mixer.setMasterLevelGain(newLevel); }
