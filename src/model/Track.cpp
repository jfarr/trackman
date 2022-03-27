#include "Track.h"
#include "MidiPlayer.h"
#include "Project.h"

#include <memory>

namespace trackman {

Track::Track(Project &project, AudioDeviceManager &deviceManager)
    : project(project), deviceManager(deviceManager), midiPlayer(*this), instrument(*this) {
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

NoteRoll *Track::addNoteRoll() {
    noteRolls.push_back(make_shared<NoteRoll>(project, *this));
    return &(*noteRolls.back());
}

NoteRoll *Track::addNoteRoll(int startPosInTicks, int endPosInTicks, const MidiMessageSequence &midiMessages) {
    noteRolls.push_back(make_shared<NoteRoll>(project, *this, startPosInTicks, endPosInTicks, midiMessages));
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
    return meteredSource == nullptr ? 0 : meteredSource->getTotalLength();
}

void Track::startRecording() {
    if (midiRecorder == nullptr) {
        recordStartPosInSeconds = project.getTransport().getCurrentPosition();
        auto *noteRoll = addNoteRoll();
        noteRoll->setSelected(true);
        project.getKeyboardState().reset();
        midiRecorder.reset(new MidiRecorder(*noteRoll, project.getKeyboardState(), deviceManager));
    }
    midiRecorder->startRecording();
}

void Track::pauseRecording() {
    if (midiRecorder != nullptr) {
        midiRecorder->stopRecording();
    }
}

void Track::stopRecording() {
    if (midiRecorder == nullptr) {
        return;
    }
    midiRecorder->onMidiMessage = nullptr;
    midiRecorder->stopRecording();
    auto &noteRoll = midiRecorder->getNoteRoll();
//    midiRecorder->printEvents();
    if (noteRoll.empty()) {
        removeNoteRoll(&noteRoll);
    }
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
        return messages.getEndTime() + 1;
    } else {
        return noteRoll.getEndTime();
    }
}

} // namespace trackman
