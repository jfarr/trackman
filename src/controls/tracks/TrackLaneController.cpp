#include "TrackLaneController.h"
#include "common/listutil.h"

TrackLaneController::TrackLaneController(
    Track &track, juce::AudioTransportSource &transport, juce::AudioFormatManager &formatManager)
    : track(track), transport(transport), trackLaneControl(track, transport), formatManager(formatManager) {
    trackLaneControl.addMouseListener(this, true);
}

void TrackLaneController::update() {
    for (std::unique_ptr<SampleThumbnail> &thumbnail : thumbnails) {
        thumbnail->removeListener(this);
    }
    thumbnails.clear();
    trackLaneControl.clear();
    track.eachSample([this](Sample &sample) {
        thumbnails.push_back(std::make_unique<SampleThumbnail>(track, sample, transport, formatManager));
        thumbnails.back()->addListener(this);
        trackLaneControl.addThumbnail(thumbnails.back().get());
    });
    trackLaneControl.update();
}

void TrackLaneController::repaint() { trackLaneControl.repaint(); }

void TrackLaneController::mouseDown(const juce::MouseEvent &event) { notifySelectionChanged(); }

void TrackLaneController::selectionChanged(Track *track) { notifySelectionChanged(); }

void TrackLaneController::sampleSelected(Track &track, Sample &sample) { notifySampleSelected(track, sample); }

void TrackLaneController::addListener(TrackListListener *listener) {
    if (!listContains(trackListListeners, listener)) {
        trackListListeners.push_front(listener);
    }
}

void TrackLaneController::removeListener(TrackListListener *listener) { trackListListeners.remove(listener); }

void TrackLaneController::notifySelectionChanged() {
    for (TrackListListener *listener : trackListListeners) {
        listener->selectionChanged(&track);
    }
}

void TrackLaneController::addListener(SampleListener *listener) {
    if (!listContains(sampleListeners, listener)) {
        sampleListeners.push_front(listener);
    }
}

void TrackLaneController::removeListener(SampleListener *listener) { sampleListeners.remove(listener); }

void TrackLaneController::notifySampleSelected(Track &track, Sample &sample) {
    for (SampleListener *listener : sampleListeners) {
        listener->sampleSelected(track, sample);
    }
}
