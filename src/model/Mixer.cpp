#include "Mixer.h"
#include "TrackList.h"
#include "common/listutil.h"

Mixer::Mixer(TrackList &trackList, juce::AudioDeviceManager &deviceManager)
    : trackList(trackList), deviceManager(deviceManager), gainSource(&mixerSource, false),
      meteredSource(&gainSource, deviceManager.getAudioDeviceSetup().sampleRate) {
    //    transportSource.setSource(&meteredSource);
}

Mixer::~Mixer() {
    //    transportSource.setSource(nullptr);
    mixerSource.removeAllInputs();
}

void Mixer::addSource(juce::PositionableAudioSource *source) {
    if (source != nullptr && !listContains(sources, source)) {
        sources.push_back(source);
        //        auto pos = transportSource.getCurrentPosition();
        mixerSource.addInputSource(source);
        source->prepareToPlay(
            deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
        //        transportSource.setPosition(pos);
    }
}

void Mixer::removeSource(juce::PositionableAudioSource *source) {
    if (source != nullptr) {
        mixerSource.removeInputSource(source);
        sources.remove(source);
    }
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

void Mixer::writeAudioFile(const juce::File &file, juce::int64 lengthInSamples) {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    int numberOfChannels = 2;
    int bitsPerSample = 16;
    file.deleteFile();
    if (auto fileStream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream())) {
        juce::WavAudioFormat wavFormat;
        if (auto writer =
                wavFormat.createWriterFor(fileStream.get(), sampleRate, numberOfChannels, bitsPerSample, {}, 0)) {
            fileStream.release();
            writer->writeFromAudioSource(gainSource, (int)(lengthInSamples));
            writer->flush();
            delete writer;
        }
    }
}

void Mixer::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    meteredSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void Mixer::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    meteredSource.getNextAudioBlock(bufferToFill);
}

void Mixer::releaseResources() { meteredSource.releaseResources(); }
