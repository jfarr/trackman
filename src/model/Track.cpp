#include "Track.h"
#include "MidiPlayer.h"
#include "Project.h"

#include <memory>

namespace trackman {

Track::Track(Project &project, AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager), midiPlayer(*this), instrument(*this) {
    //    noteRolls.push_back(make_shared<NoteRoll>(MidiMessageSequence(), 0, 0));
    midiPlayer.prepareToPlay(
        deviceManager.getAudioDeviceSetup().bufferSize, deviceManager.getAudioDeviceSetup().sampleRate);
    gainSource = make_unique<GainAudioSource>(&midiPlayer, false);
    meteredSource = make_unique<MeteredAudioSource>(gainSource.get(), deviceManager.getAudioDeviceSetup().sampleRate);

    for (auto i = 0; i < 4; ++i) {
        liveSynth.addVoice(new SineWaveVoice());
    }
    liveSynth.addSound(new SineWaveSound());
    liveSynth.setCurrentPlaybackSampleRate(deviceManager.getAudioDeviceSetup().sampleRate);
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

NoteRoll *Track::addNoteRoll(double startPosInSeconds) {
    noteRolls.push_back(make_shared<NoteRoll>(project, *this, startPosInSeconds));
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
    //    if (!hasMidi()) {
    return meteredSource == nullptr ? 0 : meteredSource->getTotalLength();
    //    } else {
    //        return midiPlayer.getTotalLength();
    //    }
}

void Track::startRecording() {
    if (midiRecorder == nullptr) {
        recordStartPosInSeconds = project.getTransport().getCurrentPosition();
        auto *noteRoll = addNoteRoll(recordStartPosInSeconds);
        noteRoll->setSelected(true);
        project.getKeyboardState().reset();
        midiRecorder.reset(new MidiRecorder(*noteRoll, project.getKeyboardState(), deviceManager));
        //        midiRecorder.setMidiMessages(noteRoll->getMidiMessages());
    }
    //    noteRoll.startRecording();
    midiRecorder->startRecording();
    //    midiPlayer.setRecording(true);
    //    recording = true;
}

void Track::pauseRecording() {
    if (midiRecorder != nullptr) {
        midiRecorder->stopRecording();
        //        midiPlayer.setRecording(false);
    }
}

void Track::stopRecording() {
    if (midiRecorder == nullptr) {
        return;
    }
    //    midiPlayer.setRecording(false);
    midiRecorder->onMidiMessage = nullptr;
    midiRecorder->stopRecording();
    auto &noteRoll = midiRecorder->getNoteRoll();
    auto pos = noteRoll.getStartPosInSeconds();
    DBG("note roll starts at: " << pos << " secs (" << noteRoll.getStartPosInTicks() << " ticks, "
                                << pos * deviceManager.getAudioDeviceSetup().sampleRate << " samples)");
    midiRecorder->printEvents();
//    auto &noteRoll = midiRecorder->getNoteRoll();
    if (noteRoll.empty()) {
        removeNoteRoll(&noteRoll);
    }
    //    DBG("set midiRecorder = nullptr");
    midiRecorder = nullptr;
}

void Track::removeNoteRoll(const NoteRoll *noteRoll) {
    for (auto i = noteRolls.begin(); i != noteRolls.end(); i++) {
        auto &p = *i;
        if (p.get() == noteRoll) {
            noteRolls.erase(i);
            break;
        }
    }
}

NoteRoll *Track::getSelectedNoteRoll() const {
    for (const shared_ptr<NoteRoll> &p : noteRolls) {
        auto &noteRoll = *p;
        if (!noteRoll.isDeleted() && noteRoll.isSelected()) {
            return &noteRoll;
        }
    }
    return nullptr;
}

void Track::eachNoteRoll(function<void(NoteRoll &noteRoll)> f) {
    for (shared_ptr<NoteRoll> &p : noteRolls) {
        auto &noteRoll = *p;
        if (!noteRoll.isDeleted()) {
            f(noteRoll);
        }
    }
}

void Track::eachCurrentMidiMessage(const NoteRoll &noteRoll, const double pos,
    function<void(const MidiMessageSequence::MidiEventHolder &eventHandle)> f) const {
    if (noteRoll.isRecording() && midiRecorder != nullptr) {
        auto messages = midiRecorder->getMidiMessages(pos);
        for (auto eventHandle : messages) {
            f(*eventHandle);
        }
    } else {
        noteRoll.eachMidiMessage(f);
    }
}

double Track::getCurrentMidiEndTimeInTicks(const NoteRoll &noteRoll, const double pos) const {
    if (midiRecorder != nullptr) {
        auto messages = midiRecorder->getMidiMessages(pos);
        return messages.getEndTime(); // - project.secondsToTicks(noteRoll.getStartPosInSeconds());
    } else {
        return noteRoll.getEndTime();
    }
}

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
    if (midiRecorder != nullptr) {
        MidiBuffer keyboardBuffer;
        midiRecorder->getKeyboardState().processNextMidiBuffer(keyboardBuffer, startSample, numSamples, true);
        buffer.addEvents(keyboardBuffer, startSample, numSamples, 0);
    }
}
//
// double Track::getMidiLengthInSeconds() const {
//    double len = 0;
//    for (const shared_ptr<NoteRoll> &p : noteRolls) {
//        auto &noteRoll = *p;
//        if (!noteRoll.isDeleted()) {
//            len = max(len, noteRoll.getEndPosInSeconds());
//        }
//    }
//    return len;
//}
//
// int64 Track::getMidiLengthInSamples() const {
//    if (!hasMidi()) {
//        return 0;
//    }
//    return getMidiLengthInSeconds() * deviceManager.getAudioDeviceSetup().sampleRate +
//           2 * deviceManager.getAudioDeviceSetup().bufferSize; // overshoot to ensure we get all note off events
//}

} // namespace trackman
