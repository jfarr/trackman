#include "MixerController.h"
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
    mixerPanel.removeAllChildren();
    for (std::unique_ptr<TrackController> &track : tracks) {
        auto trackController = new TrackController(track->getTrack(), formatManager);
        mixerPanel.addAndMakeVisible(trackController->getTrackControl());
    }
}

void MixerController::setMasterLevel(float newLevel) {
    mixer.setMasterLevel(newLevel);
    mixerPanel.getMasterTrackControl().setLevel(newLevel);
}

void MixerController::onSourceSet(std::shared_ptr<juce::PositionableAudioSource> newSource,
    std::shared_ptr<juce::PositionableAudioSource> prevSource, const bool deleteWhenRemoved,
    double sourceSampleRateToCorrectFor, int maxNumChannels) {
    if (prevSource != nullptr) {
        mixer.removeSource(prevSource);
    }
    if (newSource != nullptr) {
        mixer.addSource(newSource, deleteWhenRemoved, sourceSampleRateToCorrectFor);
    }
}

void MixerController::loopingChanged(bool shouldLoop) { mixer.setLooping(shouldLoop); }

void MixerController::masterLevelChanged(float newLevel) { mixer.setMasterLevel(newLevel); }

void MixerController::masterMuteToggled() { mixer.toggleMasterMute(); }

void MixerController::selectionChanged(Track &track, juce::Component *source) {}
