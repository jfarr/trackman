#include "MixerController.h"
#include "common/listutil.h"
#include "controls/desktop/DesktopController.h"
#include "controls/mixer/TrackController.h"

MixerController::MixerController(
    DesktopController &desktopController, TrackList &trackList, Mixer &mixer, juce::AudioFormatManager &formatManager)
    : desktopController(desktopController), trackList(trackList), mixer(mixer), formatManager(formatManager),
      mixerPanel(desktopController, trackList, mixer, mixer.getMeterSource()) {
    mixerPanel.getTransportControl().addListener(this);
    mixerPanel.getMasterTrackControl().addListener(this);
}

MixerController::~MixerController() {
    for (std::unique_ptr<TrackController> &track : tracks) {
        track->removeListener((TrackListListener *)this);
    }
    mixerPanel.getMasterTrackControl().removeListener(this);
    mixerPanel.getTransportControl().removeListener(this);
}

void MixerController::update() {
    updateAudioSource();
    for (std::unique_ptr<TrackController> &track : tracks) {
    }
    tracks.clear();
    mixerPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto controller = new TrackController(desktopController, track, formatManager);
        tracks.push_back(std::unique_ptr<TrackController>(controller));
        mixerPanel.addTrack(&controller->getTrackControl());
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

void MixerController::updateAudioSource() {
    mixer.removeAllSources();
    trackList.eachTrack([this](Track &track) {
        if (track.getSource() != nullptr) {
            DBG("MixerController::onSourceSet - add track source: " << track.getName());
            mixer.addSource(track.getSource(), track.getSampleRate(), 2);
        }
    });
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

void MixerController::selectionChanged(Track *track) { desktopController.selectionChanged(track); }
