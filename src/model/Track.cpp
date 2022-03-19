#include "Track.h"

#include <memory>

Track::Track(MidiRecorder &midiRecorder, juce::AudioDeviceManager &deviceManager)
    : deviceManager(deviceManager), midiRecorder(midiRecorder), synthAudioSource(*this) {
    synthAudioSource.prepareToPlay(
        deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    gainSource = std::make_unique<GainAudioSource>(&synthAudioSource, false);
    meteredSource =
        std::make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);
}

Track::~Track() { samples.clear(); }

void Track::setLevelGain(float newLevel) {
    level = newLevel;
    if (gainSource != nullptr) {
        gainSource->setGain(level);
    }
}

void Track::setSelected(bool newSelected) { selected = newSelected; }

Sample *Track::getSelected() const {
    for (std::shared_ptr<Sample> const &sample : samples) {
        if (!sample->isDeleted() && sample->isSelected()) {
            return sample.get();
        }
    }
    return nullptr;
}

void Track::setDeleted(bool newDeleted) {
    deleted = newDeleted;
    selected = false;
}

void Track::selectSample(Sample *newSelected) {
    eachSample([&newSelected](Sample &sample) { sample.setSelected(&sample == newSelected); });
}

void Track::moveSampleTo(Sample &sample, Track &toTrack) {
    for (auto iter = samples.begin(); iter != samples.end();) {
        if (&sample == iter->get()) {
            toTrack.samples.push_back(*iter);
            toTrack.createSamplePlayer();
            samples.erase(iter++);
        } else {
            ++iter;
        }
    }
    if (samples.empty()) {
        removeSamplePlayer();
    }
}

Sample *Track::addSample(
    const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager) {
    samples.push_back(std::make_shared<Sample>(file, startPos, endPos));
    auto sample = &(*samples.back());
    sample->loadFile(deviceManager, formatManager);
    createSamplePlayer();
    if (name == defaultName) {
        name = file.getFileName();
    }
    return &(*samples.back());
}

void Track::createSamplePlayer() {
    if (samplePlayer == nullptr) {
        samplePlayer = std::make_unique<SamplePlayer>(samples);
        gainSource = std::make_unique<GainAudioSource>(samplePlayer.get(), false);
        meteredSource =
            std::make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);
    }
}

void Track::removeSamplePlayer() {
    meteredSource = nullptr;
    gainSource = nullptr;
    samplePlayer = nullptr;
}

void Track::setMute(bool newMuted) { muted = newMuted; }

void Track::setSolo(bool newSoloed) { soloed = newSoloed; }

void Track::updateGain(bool anySoloed) {
    bool play = (!anySoloed || soloed) && !muted;
    if (gainSource != nullptr) {
        gainSource->setGain(play ? level : 0);
    }
}

void Track::eachSample(std::function<void(Sample &sample)> f) {
    for (std::shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
    }
}

juce::int64 Track::getTotalLengthInSamples() const {
    if (midiMessages.getNumEvents() == 0) {
        return meteredSource == nullptr ? 0 : meteredSource->getTotalLength();
    } else {
        return midiMessages.getEndTime() * deviceManager.getAudioDeviceSetup().sampleRate;
    }
}

void Track::startRecording() {
    if (!recording) {
//        midiRecorder.reset();
        midiRecorder.setMidiMessages(midiMessages);
    }
    midiRecorder.startRecording();
    recording = true;
}

void Track::stopRecording() {
    midiRecorder.stopRecording();
    recording = false;
    auto messages = midiRecorder.getMidiMessages();
    messages.sort();
    messages.updateMatchedPairs();
    midiMessages = messages;
}

const juce::MidiMessageSequence Track::getCurrentMidiMessages(double pos) const {
    if (recording) {
        auto messages = midiRecorder.getMidiMessages();
        std::list<juce::MidiMessage> noteOffMessages;
        for (auto i : messages) {
            if (i->message.isNoteOn() && i->noteOffObject == nullptr) {
                auto noteOff = juce::MidiMessage::noteOff(i->message.getChannel(), i->message.getNoteNumber());
                noteOff.setTimeStamp(pos);
                noteOffMessages.push_back(noteOff);
            }
        }
        for (auto noteOff : noteOffMessages) {
            messages.addEvent(noteOff);
        }
        messages.updateMatchedPairs();
        return messages;
    }
    return midiMessages;
}

void Track::setMidiMessages(const juce::MidiMessageSequence &newMessages) {
    midiMessages = newMessages;

}

void Track::processNextMidiBuffer(
    juce::MidiBuffer &buffer, const int startSample, const int numSamples, const juce::int64 currentPos) {
    // TODO: handle wrap-around
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    const double startTime = currentPos / sampleRate;
    const double endTime = startTime + numSamples / sampleRate;
    const double scaleFactor = numSamples / (double)(endTime + 1 - startTime);

    auto startIndex = midiMessages.getNextIndexAtTime(startTime);
    auto endIndex = midiMessages.getNextIndexAtTime(endTime);
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        const auto pos =
            juce::jlimit(0, numSamples - 1, juce::roundToInt((event.getTimeStamp() - startTime) * scaleFactor));
        buffer.addEvent(event, startSample + pos);
    }
    if (recording) {
        juce::MidiBuffer keyboardBuffer;
        midiRecorder.getKeyboardState().processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
        buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
    }
}
