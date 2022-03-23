#include "Track.h"
#include "Project.h"
#include "MidiPlayer.h"

#include <memory>

namespace trackman {

Track::Track(Project &project, MidiRecorder &midiRecorder, AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager), midiRecorder(midiRecorder), synthAudioSource(*this) {
    synthAudioSource.prepareToPlay(
        deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    gainSource = make_unique<GainAudioSource>(&synthAudioSource, false);
    meteredSource = make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);
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
    for (shared_ptr<Sample> const &sample : samples) {
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

Sample *Track::addSample(const File &file, double startPos, double endPos, AudioFormatManager &formatManager) {
    samples.push_back(make_shared<Sample>(file, startPos, endPos));
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
        samplePlayer = make_unique<SamplePlayer>(samples);
        gainSource = make_unique<GainAudioSource>(samplePlayer.get(), false);
        meteredSource =
            make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);
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

void Track::eachSample(function<void(Sample &sample)> f) {
    for (shared_ptr<Sample> &sample : samples) {
        if (!(*sample).isDeleted()) {
            f(*sample);
        }
    }
}

int64 Track::getTotalLengthInSamples() const {
    if (midiMessages.getNumEvents() == 0) {
        return meteredSource == nullptr ? 0 : meteredSource->getTotalLength();
    } else {
        return getMidiLengthInSamples();
    }
}

int64 Track::getMidiLengthInSamples() const {
    return project.ticksToSeconds(midiMessages.getEndTime()) * deviceManager.getAudioDeviceSetup().sampleRate +
           2 * deviceManager.getAudioDeviceSetup().bufferSize; // overshoot to ensure we get all note off events
}

void Track::startRecording() {
    if (!recording) {
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

const MidiMessageSequence Track::getCurrentMidiMessages(double pos) const {
    if (recording) {
        auto messages = midiRecorder.getMidiMessages();
        list<MidiMessage> noteOffMessages;
        for (auto i : messages) {
            if (i->message.isNoteOn() && i->noteOffObject == nullptr) {
                auto noteOff = MidiMessage::noteOff(i->message.getChannel(), i->message.getNoteNumber());
                noteOff.setTimeStamp(project.secondsToTicks(pos));
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

void Track::setMidiMessages(const MidiMessageSequence &newMessages) { midiMessages = newMessages; }

void Track::processNextMidiBuffer(
    MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    const double startTime = currentPos / sampleRate;
    const double endTime = startTime + numSamples / sampleRate;
    const double scaleFactor = numSamples / (double)(endTime + 1 - startTime);

    auto startIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(startTime));
    auto endIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(endTime));
    for (int i = startIndex; i < endIndex; i++) {
        auto p = midiMessages.getEventPointer(i);
        auto event = p->message;
        const auto pos = jlimit(0, numSamples - 1, roundToInt((event.getTimeStamp() - startTime) * scaleFactor));
        buffer.addEvent(event, event.getTimeStamp());
        //        buffer.addEvent(event, startSample + pos);
    }
    if (recording) {
        MidiBuffer keyboardBuffer;
        midiRecorder.getKeyboardState().processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
        buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
    }
}

} // namespace trackman
