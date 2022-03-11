#include "MixerController.h"
#include "common/listutil.h"
#include "controls/desktop/DesktopController.h"
#include "controls/mixer/TrackController.h"

MixerController::MixerController(
    DesktopController &desktop, TrackList &trackList, Mixer &mixer, juce::AudioFormatManager &formatManager)
    : desktop(desktop), trackList(trackList), mixer(mixer), mixerPanel(trackList, mixer, mixer.getMeterSource()),
      formatManager(formatManager) {
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
        track->removeListener((TrackListListener *)nullptr);
    }
    tracks.clear();
    mixerPanel.clear();
    trackList.eachTrack([this](Track &track) {
        auto control = new TrackControl(track);
        auto controller = new TrackController(*this, track, *control, formatManager);
        controller->addListener((TrackListListener *)this);
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

void MixerController::toggleSolo(Track &track) {
    for (std::unique_ptr<TrackController> &trackController : tracks) {
        if (&trackController->getTrack() == &track) {
            trackController->toggleSolo(track);
        }
    }
}

void MixerController::loopingChanged(bool shouldLoop) { mixer.setLooping(shouldLoop); }

void MixerController::masterLevelChanged(float newLevel) { mixer.setMasterLevelGain(newLevel); }

void MixerController::masterLevelChangeFinalized(float previousLevel) {
    desktop.masterLevelChangeFinalized(previousLevel);
}

void MixerController::masterMuteToggled() {
    mixer.toggleMasterMute();
    desktop.masterMuteToggled();
}

void MixerController::nameChanged(Track &track, juce::String newName) { desktop.trackNameChanged(track, newName); }

void MixerController::levelChangeFinalized(Track &track, float previousLevel) {
    desktop.trackLevelChangeFinalized(track, previousLevel);
}

void MixerController::muteToggled(Track &track) { desktop.trackMuteToggled(track); }

void MixerController::soloToggled(Track &track) { desktop.trackSoloToggled(track); }

void MixerController::selectionChanged(Track *track) { desktop.selectionChanged(track); }
