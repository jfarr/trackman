#include "Mixer.h"
#include "TrackList.h"
#include "common/listutil.h"

namespace trackman {

Mixer::Mixer(TrackList &trackList, AudioDeviceManager &deviceManager)
    : trackList(trackList), deviceManager(deviceManager), gainSource(&mixerSource, false),
      meteredSource(&gainSource, deviceManager.getAudioDeviceSetup().sampleRate) {}

Mixer::~Mixer() { mixerSource.removeAllInputs(); }

void Mixer::addSource(PositionableAudioSource *source) {
    if (source != nullptr && !listContains(sources, source)) {
        sources.push_back(source);
        mixerSource.addInputSource(source);
        source->prepareToPlay(
            deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    }
}

void Mixer::removeSource(PositionableAudioSource *source) {
    if (source != nullptr) {
        mixerSource.removeInputSource(source);
        sources.remove(source);
    }
}

void Mixer::removeAllSources() {
    for (PositionableAudioSource *source : sources) {
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

void Mixer::writeAudioFile(const File &file, int64 lengthInSamples) {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    int numberOfChannels = 2;
    int bitsPerSample = 16;
    file.deleteFile();
    if (auto fileStream = std::unique_ptr<FileOutputStream>(file.createOutputStream())) {
        WavAudioFormat wavFormat;
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

void Mixer::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) {
    meteredSource.getNextAudioBlock(bufferToFill);
}

void Mixer::releaseResources() { meteredSource.releaseResources(); }

} // namespace trackman
