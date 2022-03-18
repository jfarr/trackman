#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"

using json = nlohmann::json;

Project::Project(juce::AudioDeviceManager &deviceManager, MidiRecorder &midiRecorder)
    : deviceManager(deviceManager), trackList(deviceManager, midiRecorder), mixer(trackList, deviceManager) {}

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
    Track &track, const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager) {
    mixer.removeSource(track.getSource());
    auto sample = trackList.addSample(track, file, startPos, endPos, formatManager);
    mixer.addSource(track.getSource());
    return sample;
}

std::string Project::to_json() {
    json project_json = {{"horizontalScale", horizontalScale},
        {"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    project_json["tracks"] = json::array();
    juce::MidiFile midiFile;
    trackList.eachTrack([&project_json, &midiFile](Track &track) {
        midiFile.addTrack(track.getMidiMessages());
        json track_json = {{"name", track.getName().toStdString()}, {"gain", track.getLevelGain()},
            {"muted", track.isMuted()}, {"soloed", track.isSoloed()}};
        track.eachSample([&track_json](Sample &sample) {
            json sample_json = {{"file", sample.getFile().getFullPathName().toStdString()},
                {"startPos", sample.getStartPos()}, {"endPos", sample.getEndPos()}};
            track_json["samples"].push_back(sample_json);
        });
        project_json["tracks"].push_back(track_json);
    });
    juce::MemoryOutputStream out;
    midiFile.writeTo(out, 2);
    out.flush();
    auto mb = out.getMemoryBlock();
    auto encoded = juce::Base64::toBase64(mb.getData(), mb.getSize());
    project_json["midi"] = encoded.toStdString();

    return project_json.dump();
}

void Project::from_json(juce::AudioFormatManager &formatManager, std::string filename) {
    std::ifstream s(filename);
    json project_json;
    s >> project_json;
    mixer.removeAllSources();
    horizontalScale = project_json["horizontalScale"];
    mixer.setMasterLevelGain(project_json["mixer"]["gain"]);
    mixer.setMasterMute(project_json["mixer"]["muted"]);
    std::string encoded = project_json["midi"];
    juce::MemoryOutputStream out;
    juce::Base64::convertFromBase64(out, encoded);
    juce::MemoryInputStream in(out.getMemoryBlock());
    juce::MidiFile midiFile;
    midiFile.readFrom(in);

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
        track->setMidiMessages(*midiMessages);
    }
}

void Project::writeAudioFile(const juce::File &file) {
    trackList.writeAudioFile(file, mixer.getSource(), deviceManager.getAudioDeviceSetup().sampleRate, 16);
}
