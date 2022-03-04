#include "TrackController.h"
#include "common/listutil.h"

TrackController::TrackController(Track &track, TrackControl &trackControl, juce::AudioFormatManager &formatManager)
    : formatManager(formatManager), track(track), trackControl(trackControl) {
    trackControl.addListener(this);
    trackControl.setListener(this);
    trackControl.addMouseListener(this, true);
//    trackLaneControl.addMouseListener(this, true);
}

TrackController::~TrackController() {
    if (previousSource != nullptr) {
        previousSource->releaseResources();
    }
}

//void TrackController::update() {
//    trackControl.update();
//}

void TrackController::repaint() {
//    trackControl.update();
    trackControl.repaint();
}

void TrackController::fileChosen(juce::File file) {
    if (listener == nullptr) {
        return;
    }

    auto *reader = formatManager.createReaderFor(file);

    if (reader != nullptr) {
        if (previousSource != nullptr) {
            previousSource->releaseResources();
        }
        auto newSource = new juce::AudioFormatReaderSource(reader, true);
        gain = std::shared_ptr<juce::PositionableAudioSource>(new GainAudioSource(newSource, true));
        sampleRate = reader->sampleRate;
        track.setSource(gain, reader->sampleRate);
        track.setFile(file);
//        trackControl.update();
        listener->onSourceSet(gain, previousSource, false, reader->sampleRate);
        previousSource = gain;
    }
}

void TrackController::addSource() {
    if (previousSource != nullptr) {
        listener->onSourceSet(previousSource, nullptr, false, sampleRate);
    }
}

//void TrackController::removeSource() {
//    if (previousSource != nullptr) {
//        listener->onSourceSet(nullptr, previousSource, false, 0);
//    }
//}

void TrackController::levelChanged(float newLevel) {
    level = newLevel;
    auto gainSource = (GainAudioSource *)gain.get();
    if (gainSource != nullptr) {
        gainSource->setGain(newLevel);
    }
}
//void TrackController::levelChangeFinalized(TrackControl &trackControl, float previousLevel) {
//
//}

void TrackController::muteToggled() {
    muted = !muted;
    auto newLevel = (muted ? 0.0 : level);
    auto gainSource = (GainAudioSource *)gain.get();
    if (gainSource != nullptr) {
        gainSource->setGain(newLevel);
    }
}

void TrackController::mouseDown(const juce::MouseEvent &event) {
//    auto pos = event.getScreenPosition();
//    if (trackControl.getScreenBounds().contains(pos)) {
        notifySelectionChanged();
//    }
}
//
//void TrackController::setSelected(bool newSelected) {
//    selected = newSelected;
//    trackControl.setSelected(selected);
////    trackLaneControl.setSelected(selected);
//}

void TrackController::addListener(TrackListListener *listener) {
    if (!listContains(listener, listeners)) {
        listeners.push_front(listener);
    }
}

void TrackController::removeListener(TrackListListener *listener) { listeners.remove(listener); }

void TrackController::notifySelectionChanged() {
    for (TrackListListener *listener : listeners) {
        listener->selectionChanged(track, nullptr);
    }
}
