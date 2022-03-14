#include "Mixer.h"
#include "TrackList.h"
#include "common/listutil.h"

Mixer::Mixer(juce::AudioDeviceManager &deviceManager)
    : deviceManager(deviceManager), mixerSource(deviceManager), gainSource(&mixerSource, false),
      meteredSource(&gainSource, deviceManager.getAudioDeviceSetup().sampleRate) {
    transportSource.setSource(&meteredSource);
    initialized = true;
}

Mixer::~Mixer() {
    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
}

void Mixer::addSource(juce::PositionableAudioSource *source) {
    if (!listContains(sources, source)) {
        sources.push_back(source);
        auto pos = transportSource.getCurrentPosition();
        mixerSource.addInputSource(source);
        source->prepareToPlay(deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
        transportSource.setPosition(pos);
    }
}

void Mixer::removeSource(juce::PositionableAudioSource *source) {
    mixerSource.removeInputSource(source);
    sources.remove(source);
}

void Mixer::removeAllSources() {
    for (juce::PositionableAudioSource *source : sources) {
        mixerSource.removeInputSource(source);
    }
    sources.clear();
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
