#include "Track.h"
#include "MidiPlayer.h"
#include "Project.h"

#include <memory>

namespace trackman {

Track::Track(Project &project, MidiRecorder &midiRecorder, AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager), midiRecorder(midiRecorder), midiPlayer(*this) {
    //    noteRolls.push_back(make_shared<NoteRoll>(MidiMessageSequence(), 0, 0));
    midiPlayer.prepareToPlay(
        deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    gainSource = make_unique<GainAudioSource>(&midiPlayer, false);
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

Sample *Track::addSample(
    const File &file, double startPosInSeconds, double endPosInSeconds, AudioFormatManager &formatManager) {
    samples.push_back(make_shared<Sample>(file, startPosInSeconds, endPosInSeconds));
    auto sample = &(*samples.back());
    sample->loadFile(deviceManager, formatManager);
    createSamplePlayer();
    if (name == defaultName) {
        name = file.getFileName();
    }
    return sample;
}

NoteRoll *Track::addNoteRoll(
    const MidiMessageSequence &midiMessages, double startPosInSeconds, double endPosInSeconds) {
    noteRolls.push_back(make_shared<NoteRoll>(project, midiMessages));
    return &(*noteRolls.back());
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
    if (!hasMidi()) {
        return meteredSource == nullptr ? 0 : meteredSource->getTotalLength();
    } else {
        return getMidiLengthInSamples();
    }
}

void Track::startRecording() {
    if (!recording) {
        auto noteRoll = addNoteRoll(MidiMessageSequence(), 0, 0);
        midiRecorder.setMidiMessages(noteRoll->getMidiMessages());
//        midiRecorder.setMidiMessages(MidiMessageSequence());
        recordStartPosInSeconds = project.getTransport().getCurrentPosition();
    }
    midiRecorder.startRecording();
    recording = true;
}

void Track::pauseRecording() { midiRecorder.stopRecording(); }

void Track::stopRecording() {
    midiRecorder.stopRecording();
    recording = false;
    noteRolls.back()->setMidiMessages(midiRecorder.getMidiMessages());
//    auto messages = midiRecorder.getMidiMessages();
//    messages.sort();
//    messages.updateMatchedPairs();
//    //    noteRolls.pop_back();
//    noteRolls.push_back(
//        make_shared<NoteRoll>(messages, recordStartPosInSeconds, project.getTransport().getCurrentPosition()));
}

void Track::updateCurrentNoteRoll() {
    if (!noteRolls.empty()) {
        noteRolls.back()->update();
    }
}

void Track::eachNoteRoll(function<void(NoteRoll &noteRoll)> f) {
    for (shared_ptr<NoteRoll> &p : noteRolls) {
        auto &noteRoll = *p;
        if (!noteRoll.isDeleted()) {
            f(noteRoll);
        }
    }
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
    return hasMidi() ? getMidiMessages() : MidiMessageSequence();
}

//
// void Track::setMidiMessages(const MidiMessageSequence &newMessages) {
//    noteRolls.pop_back();
//    noteRolls.push_back(make_shared<NoteRoll>(newMessages, 0, 0));
//}

void Track::processNextMidiBuffer(
    MidiBuffer &buffer, const int startSample, const int numSamples, const int64 currentPos) const {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    const double startTime = currentPos / sampleRate;
    const double endTime = startTime + numSamples / sampleRate;

//    auto midiMessages = getMidiMessages();
//    auto startIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(startTime));
//    auto endIndex = midiMessages.getNextIndexAtTime(project.secondsToTicks(endTime));
//    for (int i = startIndex; i < endIndex; i++) {
//        auto p = midiMessages.getEventPointer(i);
//        auto event = p->message;
//        buffer.addEvent(event, event.getTimeStamp());
//    }
    if (recording) {
        MidiBuffer keyboardBuffer;
        midiRecorder.getKeyboardState().processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
        buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
    }
}

double Track::getMidiLengthInSeconds() const {
    double len = 0;
    for (const shared_ptr<NoteRoll> &p : noteRolls) {
        auto &noteRoll = *p;
        if (!noteRoll.isDeleted()) {
            len = max(len, noteRoll.getEndPosInSeconds());
        }
    }
    return len;
}

int64 Track::getMidiLengthInSamples() const {
    if (!hasMidi()) {
        return 0;
    }
    return project.ticksToSeconds(getMidiLengthInSeconds()) * deviceManager.getAudioDeviceSetup().sampleRate +
           2 * deviceManager.getAudioDeviceSetup().bufferSize; // overshoot to ensure we get all note off events
}

} // namespace trackman
