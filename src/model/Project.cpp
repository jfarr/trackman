#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"
#include "common/timeutil.h"

using json = nlohmann::json;

namespace trackman {

Project::Project(AudioDeviceManager &deviceManager, MidiRecorder &midiRecorder)
    : deviceManager(deviceManager), trackList(*this, deviceManager, midiRecorder), mixer(trackList, deviceManager),
      transport(mixer) {}

bool Project::isRecording() const {
    auto selected = getSelectedTrack();
    return selected == nullptr ? false : selected->isRecording();
}

int Project::secondsToTicks(double seconds) const { return TimeUtils::secondsToTicks(tempo, seconds); }

double Project::ticksToSeconds(int ticks) const { return TimeUtils::ticksToSeconds(tempo, ticks); }

double Project::measuresToSeconds(double measures) const { return timeSignature.measuresToSeconds(measures, tempo); }

double Project::secondsToMeasures(double seconds) const { return timeSignature.secondsToMeasures(seconds, tempo); }

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

string Project::to_json() {
    json project_json = {{"tempo", tempo}, {"horizontalScale", horizontalScale},
        {"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    project_json["timeSignature"] = {
        {"numerator", timeSignature.getNumerator()}, {"denominator", timeSignature.getDenominator()}};
    project_json["tracks"] = json::array();
    MidiFile midiFile;
    midiFile.setTicksPerQuarterNote(TimeUtils::ticksPerQuarterNote);
    trackList.eachTrack([&project_json, &midiFile](Track &track) {
        MidiMessageSequence messages = track.getMidiMessages();
        midiFile.addTrack(messages);
        MidiRecorder::printEvents(messages);
        json track_json = {{"name", track.getName().toStdString()}, {"gain", track.getLevelGain()},
            {"muted", track.isMuted()}, {"soloed", track.isSoloed()}};
        track.eachSample([&track_json](Sample &sample) {
            json sample_json = {{"file", sample.getFile().getFullPathName().toStdString()},
                {"startPos", sample.getStartPosInSeconds()}, {"endPos", sample.getEndPosInSeconds()}};
            track_json["samples"].push_back(sample_json);
        });
        project_json["tracks"].push_back(track_json);
    });
    MemoryOutputStream out;
    midiFile.writeTo(out, 1);
    auto mb = out.getMemoryBlock();
    auto encoded = Base64::toBase64(mb.getData(), mb.getSize());
    project_json["midi"] = encoded.toStdString();

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
    string encoded = project_json["midi"];
    MemoryOutputStream out;
    Base64::convertFromBase64(out, encoded);
    MemoryInputStream in(out.getMemoryBlock());
    MidiFile midiFile;
    midiFile.readFrom(in, true);

    trackList.clear();
    int i = 0;
    for (auto track_json : project_json["tracks"]) {
        auto track = trackList.addTrack();
        track->setName(track_json["name"]);
        track->setLevelGain(track_json["gain"]);
        trackList.setMute(*track, track_json["muted"]);
        trackList.setSolo(*track, track_json["soloed"]);
        for (auto sample_json : track_json["samples"]) {
            addSample(*track, sample_json["file"], sample_json["startPos"], sample_json["endPos"], formatManager);
        }
        auto midiMessages = midiFile.getTrack(i++);
//        track->setMidiMessages(*midiMessages);
        mixer.addSource(track->getSource());
    }
}

void Project::writeAudioFile(const File &file) { mixer.writeAudioFile(file, trackList.getTotalLengthInSamples()); }

} // namespace trackman
