#include <fstream>
#include <nlohmann/json.hpp>

#include "Project.h"

using json = nlohmann::json;

void Project::deleteTrack(Track *track) {
    auto source = track->getSource();
    if (source != nullptr) {
        mixer.removeSource(track->getSource());
    }
    trackList.deleteTrack(track);
}

Sample *Project::addSample(
    Track &track, const juce::File &file, double startPos, double endPos, juce::AudioFormatManager &formatManager) {
    auto sample = trackList.addSample(track, file, startPos, endPos, deviceManager, formatManager);
    mixer.addSource(track.getSource());
    return sample;
}

std::string Project::to_json() {
    json project_json = {{"horizontalScale", horizontalScale},
        {"mixer", {{"gain", mixer.getMasterLevelGain()}, {"muted", mixer.isMasterMuted()}}}};
    project_json["tracks"] = json::array();
    trackList.eachTrack([&project_json](Track &track) {
        json track_json = {
            {"name", track.getName().toStdString()}, {"gain", track.getLevelGain()}, {"muted", track.isMuted()}};
        track.eachSample([&track_json](Sample &sample) {
            json sample_json = {{"file", sample.getFile().getFullPathName().toStdString()},
                {"startPos", sample.getStartPos()}, {"endPos", sample.getEndPos()}};
            track_json["samples"].push_back(sample_json);
        });
        project_json["tracks"].push_back(track_json);
    });
    return project_json.dump();
}

void Project::from_json(
    juce::AudioDeviceManager &deviceManager, juce::AudioFormatManager &formatManager, std::string filename) {
    std::ifstream s(filename);
    json project_json;
    s >> project_json;
    mixer.removeAllSources();
    horizontalScale = project_json["horizontalScale"];
    mixer.setMasterLevelGain(project_json["mixer"]["gain"]);
    mixer.setMasterMute(project_json["mixer"]["muted"]);
    trackList.clear();
    auto sampleRate = deviceManager.getAudioDeviceSetup().sampleRate;
    for (auto track_json : project_json["tracks"]) {
        auto track = trackList.addTrack();
        track->setName(track_json["name"]);
        track->setLevelGain(track_json["gain"]);
        track->setMute(track_json["muted"]);
        for (auto sample_json : track_json["samples"]) {
            addSample(*track, sample_json["file"], sample_json["startPos"], sample_json["endPos"], formatManager);
        }
        //        track->loadSamples(formatManager, sampleRate);
        //        mixer.addSource(track->getSource());
    }
}
