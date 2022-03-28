#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"
#include "common/timeutil.h"

using json = nlohmann::json;

namespace trackman {

Project::Project(AudioDeviceManager &deviceManager)
    : deviceManager(deviceManager), instrumentPlayer(*this), trackList(*this, deviceManager),
      mixer(trackList, deviceManager), transport(mixer) {}

Position Project::getCurrentPosition() const {
    return positionAtTime(transport.getCurrentPosition());
}

Position Project::positionAtTime(double t) const {
    double positionInMeasures = secondsToMeasures(t);
    double measure;
    double fraction = modf(positionInMeasures, &measure);
    double beat = (fraction * timeSignature.getNumerator()) + 1;
    double unused;
    double beatFraction = modf(beat, &unused);
    return Position((int)measure, (int)beat, (int)(beatFraction * 1000));
}

double Project::timeAtPosition(Position pos) const {
    auto beat = ((double)(pos.getBeat() - 1)) / timeSignature.getNumerator();
    auto fraction = ((double)(pos.getFraction() / 1000)) / timeSignature.getNumerator();
    return measuresToSeconds(pos.getMeasure()) + measuresToSeconds(1 + beat) + measuresToSeconds(1 + fraction);
}

Synthesiser *Project::getLiveSynth() {
    auto selected = getSelectedTrack();
    return selected == nullptr ? nullptr : &selected->getLiveSynth();
}

bool Project::isRecording() const {
    auto selected = getSelectedTrack();
    return selected == nullptr ? false : selected->isRecording();
}

int Project::secondsToTicks(double seconds) const { return timeutil::secondsToTicks(tempo, seconds); }

double Project::ticksToSeconds(int ticks) const { return timeutil::ticksToSeconds(tempo, ticks); }

double Project::measuresToSeconds(double measures) const { return timeSignature.measuresToSeconds(measures, tempo); }

double Project::secondsToMeasures(double seconds) const { return timeSignature.secondsToMeasures(seconds, tempo); }

int Project::measureAt(double seconds) const {
    return (int)secondsToMeasures(seconds);
}

int Project::measureStartTick(int currentTick) const {
    auto secs = ticksToSeconds(currentTick);
    auto measure = measureAt(secs);
    auto startSecs = measuresToSeconds((double)measure);
    return secondsToTicks(startSecs);
}

int Project::measureEndTick(int currentTick) const {
    auto secs = ticksToSeconds(currentTick);
    auto measure = measureAt(secs) + 1;
    auto endSecs = measuresToSeconds((double)measure);
    return secondsToTicks(endSecs) + 1;
}

Track *Project::addTrack() {
    auto *track = trackList.addTrack();
    mixer.addSource(track->getSource());
    return track;
}

void Project::deleteTrack(Track *track) {
    auto source = track->getSource();
    if (source != nullptr) {
        mixer.removeSource(track->getSource());
    }
    trackList.deleteTrack(track);
}

Sample *Project::addSample(
    Track &track, const File &file, double startPos, double endPos, AudioFormatManager &formatManager) {
    if (!track.hasSamples()) {
        mixer.removeSource(track.getSource());
    }
    auto sample = trackList.addSample(track, file, startPos, endPos, formatManager);
    mixer.addSource(track.getSource());
    return sample;
}

NoteRoll *Project::addNoteRoll(Track &track, int startPos, int endPos, string encodedMidi) {
    if (track.hasSamples()) {
        mixer.removeSource(track.getSource());
    }
    MemoryOutputStream out;
    Base64::convertFromBase64(out, encodedMidi);
    MemoryInputStream in(out.getMemoryBlock());
    MidiFile midiFile;
    midiFile.readFrom(in, true);
    auto midiMessages = midiFile.getTrack(0);
    if (midiMessages != nullptr) {
        int startPosInTicks = startPos;
        int endPosInTicks = endPos;
        return track.addNoteRoll(startPosInTicks, endPosInTicks, *midiMessages);
    }
    return nullptr;
}

string Project::to_json() {
    json project_json = {{"tempo", tempo}, {"horizontalScale", horizontalScale},
        {"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    project_json["timeSignature"] = {
        {"numerator", timeSignature.getNumerator()}, {"denominator", timeSignature.getDenominator()}};
    project_json["tracks"] = json::array();
    trackList.eachTrack([&project_json](Track &track) {
        json track_json = {{"name", track.getName().toStdString()}, {"gain", track.getLevelGain()},
            {"muted", track.isMuted()}, {"soloed", track.isSoloed()}};
        track.eachNoteRoll([&track_json](NoteRoll &noteRoll) {
            json notes_json = {{"startPos", noteRoll.getStartPosInTicks()}, {"endPos", noteRoll.getEndPosInTicks()},
                {"midi", noteRoll.toMidiFile()}};
            track_json["notes"].push_back(notes_json);
        });
        track.eachSample([&track_json](Sample &sample) {
            json sample_json = {{"file", sample.getFile().getFullPathName().toStdString()},
                {"startPos", sample.getStartPosInSeconds()}, {"endPos", sample.getEndPosInSeconds()}};
            track_json["samples"].push_back(sample_json);
        });
        project_json["tracks"].push_back(track_json);
    });
    return project_json.dump();
}

void Project::from_json(AudioFormatManager &formatManager, string filename) {
    ifstream s(filename);
    json project_json;
    s >> project_json;
    mixer.removeAllSources();
    tempo = project_json["tempo"];
    timeSignature =
        TimeSignature(project_json["timeSignature"]["numerator"], project_json["timeSignature"]["denominator"]);
    horizontalScale = project_json["horizontalScale"];
    mixer.setMasterLevelGain(project_json["mixer"]["gain"]);
    mixer.setMasterMute(project_json["mixer"]["muted"]);

    trackList.clear();
    int i = 0;
    for (auto track_json : project_json["tracks"]) {
        auto track = trackList.addTrack();
        track->setName(track_json["name"]);
        track->setLevelGain(track_json["gain"]);
        trackList.setMute(*track, track_json["muted"]);
        trackList.setSolo(*track, track_json["soloed"]);
        for (auto notes_json : track_json["notes"]) {
            addNoteRoll(*track, notes_json["startPos"], notes_json["endPos"], notes_json["midi"]);
        }
        for (auto sample_json : track_json["samples"]) {
            addSample(*track, sample_json["file"], sample_json["startPos"], sample_json["endPos"], formatManager);
        }
        mixer.addSource(track->getSource());
    }
}

void Project::writeAudioFile(const File &file) { mixer.writeAudioFile(file, trackList.getTotalLengthInSamples()); }

void Project::printEvents(const MidiMessageSequence &midiMessages) const {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    for (auto i = midiMessages.begin(); i != midiMessages.end(); i++) {
        printEvent((*i)->message, sampleRate);
    }
}

void Project::printEvents(const MidiBuffer &buffer) const {
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    for (auto i = buffer.begin(); i != buffer.end(); i++) {
        printEvent((*i).getMessage(), sampleRate);
    }
}

void Project::printEvent(const MidiMessage &event, double sampleRate) const {
    auto t = ticksToSeconds(event.getTimeStamp());
    DBG(String("note ") + (event.isNoteOn() ? "on" : "off") + " event at time "
        << t << " (" << event.getTimeStamp() << " ticks, " << (t * sampleRate)
        << " samples): noteNumber=" << event.getNoteNumber() << " velocity=" << event.getVelocity());
}

} // namespace trackman
