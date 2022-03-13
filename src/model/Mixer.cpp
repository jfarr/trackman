#include "Mixer.h"
#include "TrackList.h"
#include "common/listutil.h"

Mixer::Mixer(TrackList &trackList, double sampleRate)
    : trackList(trackList), mixerSource(sampleRate), gainSource(&mixerSource, false),
      meteredSource(gainSource, sampleRate) {
    transportSource.setSource(&meteredSource);
    initialized = true;
}

Mixer::~Mixer() {
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
}

void Mixer::addSource(juce::PositionableAudioSource *source) {
    if (!listContains(sources, source)) {
        DBG("Mixer::addSource - add source with length: " << source->getTotalLength());
        sources.push_back(source);
        auto pos = transportSource.getCurrentPosition();
        mixerSource.addInputSource(source);
        //        trackList.adjustTrackLengths();
        transportSource.setPosition(pos);
        DBG("Mixer::addSource - set position: " << pos);
        DBG("Mixer::addSource - length: " << transportSource.getTotalLength());
    }
}

void Mixer::removeAllSources() {
    for (juce::PositionableAudioSource *source : sources) {
        mixerSource.removeInputSource(source);
    }
    sources.clear();
    //    trackList.adjustTrackLengths();
}

void Mixer::setMasterLevelGain(float newLevel) {
    level = newLevel;
    gainSource.setGain(newLevel);
}

void Mixer::setMasterMute(bool newMuted) {
    muted = newMuted;
    gainSource.setGain((muted ? 0 : level));
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    if (initialized) {
        //        trackList.adjustTrackLengths();
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (initialized) {
        transportSource.getNextAudioBlock(bufferToFill);
    }
}

void Mixer::releaseResources() {
    if (initialized) {
        transportSource.releaseResources();
    }
}
